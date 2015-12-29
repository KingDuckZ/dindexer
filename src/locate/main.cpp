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

#include "commandline.hpp"
#include "postgre_locate.hpp"
#include "dindexer-common/settings.hpp"
#include "dindexerConfig.h"
#include <iostream>
#include <ciso646>
#include <iterator>
#include <algorithm>

namespace din {
	std::ostream& operator<< (std::ostream& parStream, const LocatedItem& parItem) {
		parStream << parItem.group_id << '\t' << parItem.id << '\t' << parItem.path;
		return parStream;
	}
} //namespace din

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

	const auto results = din::locate_in_db(settings.db, vm["substring"].as<std::string>(), not not vm.count("case-insensitive"));
	std::copy(results.begin(), results.end(), std::ostream_iterator<din::LocatedItem>(std::cout, "\n"));
	return 0;
}