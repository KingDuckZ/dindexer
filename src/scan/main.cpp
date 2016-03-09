/* Copyright 2015, 2016, Michele Santullo
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
#include "dindexer-machinery/machinery_info.hpp"
#include "dindexer-common/common_info.hpp"
#include "dindexer-common/settings.hpp"
#include "commandline.hpp"
#include "dbbackend.hpp"
#include "dindexer-machinery/scantask/dirtree.hpp"
#include "dindexer-machinery/scantask/mediatype.hpp"
#include "dindexer-machinery/scantask/hashing.hpp"
#include "dindexer-machinery/scantask/contenttype.hpp"
#include "dindexer-machinery/scantask/mime.hpp"
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

//namespace {
//	void run_hash_calculation ( mchlib::Indexer& parIndexer, bool parShowProgress );
//	bool add_to_db ( const std::vector<mchlib::FileRecordData>& parData, const std::string& parSetName, char parType, char parContent, const dinlib::SettingsDB& parDBSettings, bool parForce=false );
//} //unnamed namespace

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

	const std::string search_path(vm["search-path"].as<std::string>());
	std::shared_ptr<mchlib::scantask::DirTree> scan_dirtree(new mchlib::scantask::DirTree(search_path));
	std::shared_ptr<mchlib::scantask::MediaType> media_type(new mchlib::scantask::MediaType((vm.count("type") ? vm["type"].as<char>() : 'O'), vm.count("type"), search_path));
	std::shared_ptr<mchlib::scantask::Hashing> hashing(new mchlib::scantask::Hashing(scan_dirtree, true));
	std::shared_ptr<mchlib::scantask::ContentType> content_type(new mchlib::scantask::ContentType(scan_dirtree, media_type));
	std::shared_ptr<mchlib::scantask::Mime> mime(new mchlib::scantask::Mime(scan_dirtree));

	std::cout << "Content type: " << mchlib::content_type_to_char(content_type->get_or_create()) << std::endl;
	mime->get_or_create();

	const auto& hashes = hashing->get_or_create();
	for (const auto& hash : hashes) {
		std::cout << '"' << hash.path << "\" -> " << mchlib::tiger_to_string(hash.hash) << " mime: " << hash.mime_type << "\n";
	}

	return 0;

#if defined(WITH_MEDIA_AUTODETECT)
	//char set_type;
	//if (0 == vm.count("type")) {
	//	std::cout << "Analyzing disc... ";
	//	try {
	//		const auto guessed_type = mchlib::guess_media_type(std::string(search_path));
	//		set_type = guessed_type;
	//		std::cout << "Setting type to " << set_type << " ("
	//			<< dinlib::media_type_to_str(guessed_type) << ")\n";
	//	}
	//	catch (const std::runtime_error& e) {
	//		std::cout << '\n';
	//		std::cerr << e.what();
	//		return 1;
	//	}
	//}
	//else {
	//	set_type = vm["type"].as<char>();
	//}
	//std::unique_ptr<mchlib::scantask::MediaAutodetect> media_autodetector(
		//new mchlib::scantask::MediaAutodetect(vm["type"].as<char>())
	//);
#else
	const char set_type = vm["type"].as<char>();
#endif

	std::cout << "constructing...\n";

	//indexer.ignore_read_errors(vm.count("ignore-errors") > 0);
	//if (verbose) {
	//	std::cout << "Fetching items list...\n";
	//}

	//if (indexer.empty()) {
	//	std::cerr << "Nothing found at the given location, quitting\n";
	//	return 1;
	//}
	//else {
	{
		//run_hash_calculation(indexer, verbose);

		//TODO: guess_content_type() relies on FileRecordData::path being set to
		//the relative path already. Unfortunately at this point it just got
		//default-initialized to be the same as abs_path, so for a video DVD, for
		//example, it's going to be like "/mnt/cdrom/VIDEO_TS" instead of just
		//"VIDEO_TS". This will cause guess_content_type() to miss. Relative
		//paths are populated at the end of calculate_hash(), so until I come up
		//with a better system I'm just moving content detection to after hash
		//calculation.
		//const auto set_type_casted = dinlib::char_to_media_type(set_type);
		//const mchlib::ContentTypes content = mchlib::guess_content_type(set_type_casted, indexer.record_data());
		//const char content_type = mchlib::content_type_to_char(content);

		if (verbose) {
			std::cout << "Writing to database...\n";
		}
		//if (not add_to_db(indexer.record_data(), vm["setname"].as<std::string>(), set_type, content_type, settings.db)) {
		//	std::cerr << "Not written to DB, likely because a set with the same hash already exists\n";
		//}
	}
	return 0;
}

//namespace {
//	void run_hash_calculation (mchlib::Indexer& parIndexer, bool parShowProgress) {
//		if (parIndexer.empty()) {
//			return;
//		}
//
//#if !defined(WITH_PROGRESS_FEEDBACK)
//		parShowProgress = false;
//#endif
//		if (not parShowProgress) {
////Hashing file /mnt/cdrom/Sacred 2/Fallen Angel/UK/Sacred.2.Fallen.Angel-ArenaBG/DISC2/S2DISC2.md1... 512c201321ed01cc2a82c9f80bfeaaa673bc8eb3cea4e5c1
////terminate called after throwing an instance of 'std::ios_base::failure'
////what():  basic_filebuf::xsgetn error reading the file
////Hashing file /mnt/cdrom/Sacred 2/Fallen Angel/UK/Sacred.2.Fallen.Angel-ArenaBG/DISC2/S2DISC2.mdf...Annullato
//			parIndexer.calculate_hash();
//		}
//#if defined(WITH_PROGRESS_FEEDBACK)
//		else {
//			typedef std::ostream_iterator<char> cout_iterator;
//
//			std::cout << "Processing";
//			std::cout.flush();
//			const auto total_items = parIndexer.total_items();
//			std::thread hash_thread(&mchlib::Indexer::calculate_hash, &parIndexer);
//			std::mutex progress_print;
//			std::size_t clear_size = 0;
//			const auto digit_count = static_cast<std::size_t>(std::log10(static_cast<double>(total_items))) + 1;
//			do {
//				//TODO: fix this steaming pile of crap
//				//std::unique_lock<std::mutex> lk(progress_print);
//				//parIndexer.step_notify().wait(lk);
//				std::cout << '\r';
//				std::fill_n(cout_iterator(std::cout), clear_size, ' ');
//				std::cout << '\r';
//				{
//					std::ostringstream oss;
//					const auto item_index = std::min(total_items - 1, parIndexer.processed_items());
//					oss << "Processing file "
//						<< std::setw(digit_count) << std::setfill(' ') << (item_index + 1)
//						<< " of " << total_items << " \"" << parIndexer.current_item() << '"';
//					const auto msg(oss.str());
//					clear_size = msg.size();
//					std::cout << msg;
//					std::cout.flush();
//				}
//			} while (false); //parIndexer.processed_items() != total_items);
//
//			hash_thread.join();
//			if (parIndexer.processed_items() > 0) {
//				std::cout << '\n';
//			}
//		}
//#endif
//	}
//
//	bool add_to_db (const std::vector<mchlib::FileRecordData>& parData, const std::string& parSetName, char parType, char parContentType, const dinlib::SettingsDB& parDBSettings, bool parForce) {
//		using mchlib::FileRecordData;
//		using mchlib::SetRecordDataFull;
//		using mchlib::SetRecordData;
//
//		if (not parForce) {
//			const auto& first_hash = parData.front().hash;
//			FileRecordData itm;
//			SetRecordDataFull set;
//			const bool already_in_db = din::read_from_db(itm, set, parDBSettings, first_hash);
//			if (already_in_db) {
//				return false;
//			}
//		}
//
//		SetRecordData set_data {parSetName, parType, parContentType };
//		const auto app_signature = dinlib::dindexer_signature();
//		const auto lib_signature = mchlib::lib_signature();
//		const std::string signature = std::string(app_signature.data(), app_signature.size()) + "/" + std::string(lib_signature.data(), lib_signature.size());
//		din::write_to_db(parDBSettings, parData, set_data, signature);
//		return true;
//	}
//} //unnamed namespace
