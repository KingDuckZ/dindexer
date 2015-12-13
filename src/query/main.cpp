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
#include <iostream>
#include <ciso646>
#include <string>
#include <vector>
#include <boost/algorithm/string/join.hpp>

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

	if (not vm.count("query-command")) {
		std::cerr << "No query specified, please use --help for usage instructions\n";
		return 2;
	}

	const auto query_command = boost::algorithm::join(vm["query-command"].as<std::vector<std::string>>(), " ");
	std::cout << "Query: " << query_command << '\n';
	return 0;
}

