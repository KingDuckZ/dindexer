/* Copyright 2016, Michele Santullo
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
#include "commandprocessor.hpp"
#include "dirmanager.hpp"
#include <iostream>
#include <ciso646>
#include <string>
#include <vector>
#include <cassert>

namespace {
	void do_navigation ( void );

	bool on_exit ( void );
	void on_pwd ( const din::DirManager& parDirMan );
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

	do_navigation();
	return 0;
}

namespace {
	bool on_exit() {
		return true;
	}
	void on_pwd (const din::DirManager& parDirMan) {
		std::cout << parDirMan.to_string() << '\n';
	}

	void do_navigation() {
		auto& inp = std::cin;

		bool running = true;
		std::string curr_line;
		din::CommandProcessor proc;
		din::DirManager dir_man;
		proc.add_command("exit", &on_exit, 0);
		proc.add_command("cd", std::function<void(const std::string&)>(std::bind(&din::DirManager::push_piece, &dir_man, std::placeholders::_1)), 1);
		proc.add_command("pwd", std::function<void()>(std::bind(&on_pwd, std::ref(dir_man))), 0);
		do {
			do {
				std::getline(inp, curr_line);
			} while (curr_line.empty());
			running = proc.exec_command(curr_line);
		} while (running);
	}
} //unnamed namespace
