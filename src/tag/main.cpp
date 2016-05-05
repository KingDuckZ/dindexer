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
#include "split_tags.hpp"
#include <iostream>
#include <ciso646>

namespace {
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

	if (not vm.count("ids")) {
		std::cerr << "No IDs specified\n";
		return 2;
	}

	const auto ids = vm["ids"].as<std::vector<uint64_t>>();
	for (auto id : ids) {
		std::cout << id << '\n';
	}

	const auto master_tags_string = vm["tags"].as<std::string>();
	std::vector<boost::string_ref> tags = din::split_tags(master_tags_string);

	for (auto tag : tags) {
		std::cout << '"' << tag << "\"\n";
	}
	return 0;
}
