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
#include <iostream>
#include <ciso646>
#include <ios>
#include <vector>
#include <cstdint>

namespace {
	bool confirm_delete (const dindb::IDDescMap& parMap) {
		for (const auto& itm : parMap) {
			std::cout << "ID " << itm.first << '\t' << itm.second << '\n';
		}
		std::cout << "Confirm deleting these sets? (Y/n)" << std::endl;
		std::string answer;
		//see also: http://www.lb-stuff.com/user-input
		std::cin >> std::noskipws >> answer >> std::skipws;

		return (answer.empty() or "y" == answer or "Y" == answer);
	}

	bool always_delete (const dindb::IDDescMap&) {
		return true;
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
	catch (const dinlib::ValidationError& err) {
		std::cerr << "Unrecognized id \"" << err.raw_value() << "\", see --help for usage details\n";
		return 2;
	}

	dinlib::Settings settings;
	try {
		dinlib::load_settings(CONFIG_FILE_PATH, settings);
	}
	catch (const std::runtime_error& err) {
		std::cerr << "Can't load settings from " << CONFIG_FILE_PATH << ":\n";
		std::cerr << err.what() << '\n';
		return 1;
	}

	if (not vm.count("groupid")) {
		std::cerr << "No IDs specified\n";
		return 2;
	}

	const auto ids = vm["groupid"].as<std::vector<uint32_t>>();
	auto confirm_func = (vm.count("confirm") ? &always_delete : &confirm_delete);
	settings.backend_plugin.backend().delete_group(ids, confirm_func);
	return 0;
}

