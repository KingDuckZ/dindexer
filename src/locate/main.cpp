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
#include "backends/postgresql/locate.hpp"
#include "dindexer-common/settings.hpp"
#include "dindexer-common/split_tags.hpp"
#include "dindexerConfig.h"
#include "hash.hpp"
#include "glob2regex/glob2regex.hpp"
#include <iostream>
#include <ciso646>
#include <iterator>
#include <algorithm>

namespace din {
	std::ostream& operator<< (std::ostream& parStream, const LocatedItem& parItem) {
		parStream << parItem.group_id << '\t' << parItem.id << '\t' << parItem.path;
		return parStream;
	}

	std::ostream& operator<< (std::ostream& parStream, const LocatedSet& parItem) {
		const decltype(parItem.dir_count) one = 1;
		const auto dircount = std::max(parItem.dir_count, one) - one;

		parStream << parItem.id << "\t\"" << parItem.desc << "\"\t" <<
			'\t' << parItem.files_count << '\t' << dircount;
		return parStream;
	}
} //namespace din

namespace {
	std::vector<boost::string_ref> extract_tags (const boost::program_options::variables_map& parVM) {
		if (not parVM.count("tags"))
			return std::vector<boost::string_ref>();
		else
			return dinlib::split_tags(parVM["tags"].as<std::string>());
	}
} //unnamed namespace

int main (int parArgc, char* parArgv[]) {
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

	if (not vm.count("substring")) {
		std::cerr << "Missing search parameter, please use --help for usage instructions.\n";
		return 2;
	}

	dinlib::Settings settings;
	{
		const bool loaded = dinlib::load_settings(CONFIG_FILE_PATH, settings);
		if (not loaded) {
			std::cerr << "Can't load settings from " << CONFIG_FILE_PATH << ", quitting\n";
			return 1;
		}
	}

	if (vm.count("set")) {
		const auto results = din::locate_sets_in_db(settings.db, vm["substring"].as<std::string>(), not not vm.count("case-insensitive"));
		std::copy(results.begin(), results.end(), std::ostream_iterator<din::LocatedSet>(std::cout, "\n"));
	}
	else {
		std::vector<din::LocatedItem> results;
		const std::vector<boost::string_ref> tags = extract_tags(vm);

		if (vm.count("byhash")) {
			const auto hash = din::hash(vm["substring"].as<std::string>());
			results = din::locate_in_db(settings.db, hash, tags);
		}
		else {
			const auto search_regex = g2r::convert(vm["substring"].as<std::string>(), not vm.count("case-insensitive"));
			results = din::locate_in_db(settings.db, search_regex, tags);
		}
		std::copy(results.begin(), results.end(), std::ostream_iterator<din::LocatedItem>(std::cout, "\n"));
	}
	return 0;
}
