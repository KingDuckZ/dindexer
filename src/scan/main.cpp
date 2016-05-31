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
#include "dindexer-machinery/scantask/dirtree.hpp"
#include "dindexer-machinery/scantask/mediatype.hpp"
#include "dindexer-machinery/scantask/hashing.hpp"
#include "dindexer-machinery/scantask/contenttype.hpp"
#include "dindexer-machinery/scantask/mime.hpp"
#include "dindexer-machinery/scantask/generalfiller.hpp"
#include "dindexer-machinery/scantask/setbasic.hpp"
#include <iostream>
#include <iomanip>
#include <ciso646>
#if defined(WITH_PROGRESS_FEEDBACK)
#	include <sstream>
#endif

namespace {
	bool add_to_db ( const std::vector<mchlib::FileRecordData>& parData, const mchlib::SetRecordDataFull& parSet, dindb::Backend& parDB, bool parForce=false );
#if defined(WITH_PROGRESS_FEEDBACK)
	void print_progress ( const boost::string_ref parPath, uint64_t parFileBytes, uint64_t parTotalBytes, uint32_t parFileNum, std::size_t& parClearCount );
#endif
} //unnamed namespace

namespace stask = mchlib::scantask;
#if defined(WITH_PROGRESS_FEEDBACK)
namespace ph = std::placeholders;
#endif

int main (int parArgc, char* parArgv[]) {
	using boost::program_options::variables_map;
	using FileRecordDataFiller = stask::GeneralFiller<stask::DirTree::PathList>;
	using SetRecordDataFiller = stask::GeneralFiller<mchlib::SetRecordDataFull>;

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
	//const bool verbose = (0 == vm.count("quiet"));
#else
	//const bool verbose = false;
#endif

	dinlib::Settings settings;
	{
		const bool loaded = dinlib::load_settings(CONFIG_FILE_PATH, settings);
		if (not loaded) {
			std::cerr << "Can't load settings from " << CONFIG_FILE_PATH << ", quitting\n";
			return 1;
		}
	}
	//TODO: throw if plugin loading failed
	assert(settings.backend_plugin.name() == settings.backend_name);
	assert(settings.backend_plugin.is_loaded());

	bool ignore_read_errors = (vm.count("ignore-errors") > 0);
	const std::string search_path(vm["search-path"].as<std::string>());
	const char def_media_type = (vm.count("type") ? vm["type"].as<char>() : 'O');

	std::shared_ptr<stask::SetBasic> setbasic(new stask::SetBasic(std::string(vm["setname"].as<std::string>())));
	std::shared_ptr<stask::DirTree> scan_dirtree(new stask::DirTree(search_path));
	std::shared_ptr<stask::MediaType> media_type(new stask::MediaType(setbasic, def_media_type, vm.count("type"), search_path));
	std::shared_ptr<stask::Hashing> hashing(new stask::Hashing(scan_dirtree, ignore_read_errors));
	std::shared_ptr<stask::ContentType> content_type(new stask::ContentType(setbasic, scan_dirtree, media_type));
	std::shared_ptr<stask::Mime> mime(new stask::Mime(scan_dirtree));
	std::shared_ptr<FileRecordDataFiller> filerecdata(new FileRecordDataFiller(mime, hashing));
	std::shared_ptr<SetRecordDataFiller> setrecdata(new SetRecordDataFiller(media_type, content_type));

#if defined(WITH_PROGRESS_FEEDBACK)
	std::size_t local_size_t = 0;
	hashing->set_progress_callback(
		[local_size_t](const boost::string_ref parPath, uint64_t parFileBytes, uint64_t parTotalBytes, uint32_t parFileNum) mutable {
			print_progress(parPath, parFileBytes, parTotalBytes, parFileNum, local_size_t);
		}
	);
#endif

	const bool added_to_db = add_to_db(filerecdata->get_or_create(), setrecdata->get_or_create(), settings.backend_plugin.backend());
#if defined(WITH_PROGRESS_FEEDBACK)
		std::cout << '\n';
#endif
	if (not added_to_db) {
		std::cerr << "Not written to DB, likely because a set with the same hash already exists\n";
	}
	return 0;
}

namespace {
	bool add_to_db (const std::vector<mchlib::FileRecordData>& parData, const mchlib::SetRecordDataFull& parSet, dindb::Backend& parDB, bool parForce) {
		using mchlib::FileRecordData;
		using mchlib::SetRecordDataFull;
		using mchlib::SetRecordData;

		if (not parForce) {
			const auto& first_hash = parData.front().hash;
			FileRecordData itm;
			SetRecordDataFull set;
			const bool already_in_db = parDB.search_file_by_hash(itm, set, first_hash);
			if (already_in_db) {
				return false;
			}
		}

		const SetRecordData& set_data {parSet.name, parSet.type, parSet.content_type };
		const auto app_signature = dinlib::dindexer_signature();
		const auto lib_signature = mchlib::lib_signature();
		const std::string signature = std::string(app_signature.data(), app_signature.size()) + "/" + std::string(lib_signature.data(), lib_signature.size());
		parDB.write_files(parData, set_data, signature);
		return true;
	}

#if defined(WITH_PROGRESS_FEEDBACK)
	void print_progress (const boost::string_ref parPath, uint64_t /*parFileBytes*/, uint64_t parTotalBytes, uint32_t parFileNum, std::size_t& parClearCount) {
		std::ostringstream oss;

		std::fill_n(std::ostream_iterator<char>(std::cout), parClearCount, ' ');
		std::cout << '\r';
		oss << "Hashing file " << parFileNum << " \"" << parPath << "\" (" << parTotalBytes << " bytes hashed)";
		const auto msg = oss.str();
		parClearCount = msg.size();

		std::cout << msg << '\r';
		std::cout.flush();
	}
#endif
} //unnamed namespace
