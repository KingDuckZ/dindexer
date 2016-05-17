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

#include "commandline.hpp"
#include "dindexer-common/settings.hpp"
#include "dindexerConfig.h"
#include "tag_postgres.hpp"
#include "dindexer-common/split_tags.hpp"
#include "glob2regex/glob2regex.hpp"
#include <iostream>
#include <ciso646>
#include <algorithm>
#include <boost/lexical_cast.hpp>

namespace {
} //unnamed namespace

int main (int parArgc, char* parArgv[]) {
	using boost::program_options::variables_map;
	using boost::lexical_cast;
	using boost::string_ref;

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

	const bool id_mode = static_cast<bool>(vm.count("ids"));
	const bool glob_mode = static_cast<bool>(vm.count("glob"));
	if (not id_mode and not glob_mode) {
		std::cerr << "No IDs or glob specified\n";
		return 2;
	}
	else if (id_mode and glob_mode) {
		std::cerr << "Can't specify both a glob and IDs, please only use one of them\n";
		return 2;
	}
	assert(id_mode xor glob_mode);

	dinlib::Settings settings;
	{
		const bool loaded = dinlib::load_settings(CONFIG_FILE_PATH, settings);
		if (not loaded) {
			std::cerr << "Can't load settings from " << CONFIG_FILE_PATH << ", quitting\n";
			return 1;
		}
	}

	const auto master_tags_string = vm["tags"].as<std::string>();
	const std::vector<boost::string_ref> tags = dinlib::split_tags(master_tags_string);

	if (id_mode) {
		auto ids_string = dinlib::split_tags(vm["ids"].as<std::string>());
		std::vector<uint64_t> ids;
		ids.reserve(ids_string.size());
		std::transform(ids_string.begin(), ids_string.end(), std::back_inserter(ids), &lexical_cast<uint64_t, string_ref>);
		din::tag_files(settings.db, ids, tags);
	}
	else if (glob_mode) {
		const auto regex = g2r::convert(vm["glob"].as<std::string>());
		din::tag_files(settings.db, regex, true, tags);
	}
	else {
		assert(false);
		return 1;
	}

	return 0;
}
