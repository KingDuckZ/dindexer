/* Copyright 2015, Michele Santullo
 * This file is part of "dindexer".
 *
 * "dindexer" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "dindexer" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "dindexer".  If not, see <http://www.gnu.org/licenses/>.
 */

#if defined(WITH_PROGRESS_FEEDBACK) && !defined(NDEBUG)
#	undef WITH_PROGRESS_FEEDBACK
#endif

#include "dindexer-machinery/recorddata.hpp"
#include "dindexerConfig.h"
#include "dindexer-machinery/filesearcher.hpp"
#include "dindexer-machinery/indexer.hpp"
#include "dindexer-common/settings.hpp"
#include "commandline.hpp"
#include "dbbackend.hpp"
#include <iostream>
#include <iomanip>
#include <ciso646>
#include <sstream>
#include <algorithm>
#include <iterator>
#if defined(WITH_PROGRESS_FEEDBACK)
#	include <thread>
#	include <mutex>
#	include <condition_variable>
#endif

namespace {
	void run_hash_calculation ( din::Indexer& parIndexer, bool parShowProgress );
	bool add_to_db ( const std::vector<din::FileRecordData>& parData, const std::string& parSetName, char parType, const dinlib::SettingsDB& parDBSettings, bool parForce=false );
} //unnamed namespace

int main (int parArgc, char* parArgv[]) {
	using std::placeholders::_1;
	using std::placeholders::_2;
	using boost::program_options::variables_map;

	variables_map vm;
	try {
		if (din::parse_commandline(parArgc, parArgv, vm)) {
			return 0;
		}
	}
	catch (const std::invalid_argument& err) {
		std::cerr << err.what() << "\nUse --help for help" << std::endl;
		return 2;
	}
	const std::string search_path(vm["search-path"].as<std::string>());
#if defined(WITH_PROGRESS_FEEDBACK)
	const bool verbose = (0 == vm.count("quiet"));
#else
	const bool verbose = false;
#endif

	dinlib::Settings settings;
	{
		const bool loaded = dinlib::load_settings(CONFIG_FILE_PATH, settings);
		if (not loaded) {
			std::cerr << "Can't load settings from " << CONFIG_FILE_PATH << ", quitting\n";
			return 1;
		}
	}

#if defined(WITH_MEDIA_AUTODETECT)
	char set_type;
	if (0 == vm.count("type")) {
		std::cout << "Analyzing disc... ";
		try {
			const auto guessed_type = din::guess_media_type(std::string(search_path));
			set_type = guessed_type;
			std::cout << "Setting type to " << set_type << " ("
				<< dinlib::media_type_to_str(guessed_type) << ")\n";
		}
		catch (const std::runtime_error& e) {
			std::cout << '\n';
			std::cerr << e.what();
			return 1;
		}
	}
	else {
		set_type = vm["type"].as<char>();
	}
#else
	const char set_type = vm["type"].as<char>();
#endif

	std::cout << "constructing...\n";

	din::Indexer indexer;
	indexer.ignore_read_errors(vm.count("ignore-errors") > 0);
	fastf::FileSearcher searcher(search_path);
	fastf::FileSearcher::ConstCharVecType ext, ignore;
	searcher.SetFollowSymlinks(true);
	searcher.SetCallback(fastf::FileSearcher::CallbackType(std::bind(&din::Indexer::add_path, &indexer, _1, _2)));
	searcher.Search(ext, ignore);
	if (verbose) {
		std::cout << "Fetching items list...\n";
	}

	if (indexer.empty()) {
		std::cerr << "Nothing found at the given location, quitting\n";
		return 1;
	}
	else {
		run_hash_calculation(indexer, verbose);
		if (verbose) {
			std::cout << "Writing to database...\n";
		}
		if (not add_to_db(indexer.record_data(), vm["setname"].as<std::string>(), set_type, settings.db)) {
			std::cerr << "Not written to DB, likely because a set with the same hash already exists\n";
		}
	}
	return 0;
}

namespace {
	void run_hash_calculation (din::Indexer& parIndexer, bool parShowProgress) {
		if (parIndexer.empty()) {
			return;
		}

#if !defined(WITH_PROGRESS_FEEDBACK)
		parShowProgress = false;
#endif
		if (not parShowProgress) {
//Hashing file /mnt/cdrom/Sacred 2/Fallen Angel/UK/Sacred.2.Fallen.Angel-ArenaBG/DISC2/S2DISC2.md1... 512c201321ed01cc2a82c9f80bfeaaa673bc8eb3cea4e5c1
//terminate called after throwing an instance of 'std::ios_base::failure'
//what():  basic_filebuf::xsgetn error reading the file
//Hashing file /mnt/cdrom/Sacred 2/Fallen Angel/UK/Sacred.2.Fallen.Angel-ArenaBG/DISC2/S2DISC2.mdf...Annullato
			parIndexer.calculate_hash();
		}
#if defined(WITH_PROGRESS_FEEDBACK)
		else {
			typedef std::ostream_iterator<char> cout_iterator;

			std::cout << "Processing";
			std::cout.flush();
			const auto total_items = parIndexer.total_items();
			std::thread hash_thread(&din::Indexer::calculate_hash, &parIndexer);
			std::mutex progress_print;
			std::size_t clear_size = 0;
			const auto digit_count = static_cast<std::size_t>(std::log10(static_cast<double>(total_items))) + 1;
			do {
				std::unique_lock<std::mutex> lk(progress_print);
				parIndexer.step_notify().wait(lk);
				std::cout << '\r';
				std::fill_n(cout_iterator(std::cout), clear_size, ' ');
				std::cout << '\r';
				{
					std::ostringstream oss;
					const auto item_index = std::min(total_items - 1, parIndexer.processed_items());
					oss << "Processing file "
						<< std::setw(digit_count) << std::setfill(' ') << (item_index + 1)
						<< " of " << total_items << " \"" << parIndexer.current_item() << '"';
					const auto msg(oss.str());
					clear_size = msg.size();
					std::cout << msg;
					std::cout.flush();
				}
			} while (parIndexer.processed_items() != total_items);

			hash_thread.join();
			if (parIndexer.processed_items() > 0) {
				std::cout << '\n';
			}
		}
#endif
	}

	bool add_to_db (const std::vector<din::FileRecordData>& parData, const std::string& parSetName, char parType, const dinlib::SettingsDB& parDBSettings, bool parForce) {
		using din::FileRecordData;
		using din::SetRecordDataFull;
		using din::SetRecordData;

		if (not parForce) {
			const auto& first_hash = parData.front().hash;
			FileRecordData itm;
			SetRecordDataFull set;
			const bool already_in_db = read_from_db(itm, set, parDBSettings, first_hash);
			if (already_in_db) {
				return false;
			}
		}

		SetRecordData set_data {parSetName, parType};
		write_to_db(parDBSettings, parData, set_data);
		return true;
	}
} //unnamed namespace
