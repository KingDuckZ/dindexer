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
#include "commandprocessor.hpp"
#include "dindexer-common/settings.hpp"
#include "entrypath.hpp"
#include "dindexerConfig.h"
#include "linereader.hpp"
#include "listdircontent.hpp"
#include <iostream>
#include <ciso646>
#include <string>
#include <vector>
#include <boost/range/algorithm/copy.hpp>

namespace {
	void do_navigation ( dindb::Backend& parDB );

	bool on_exit ( void );
	void on_pwd ( const din::EntryPath& parDirMan );
	void on_ls ( const din::ListDirContent& parLS, const din::EntryPath& parDirMan );
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

	dinlib::Settings settings;
	try {
		dinlib::load_settings(CONFIG_FILE_PATH, settings);
	}
	catch (const std::runtime_error& err) {
		std::cerr << "Can't load settings from " << CONFIG_FILE_PATH << ":\n";
		std::cerr << err.what() << '\n';
		return 1;
	}

	do_navigation(settings.backend_plugin.backend());
	return 0;
}

namespace {
	bool on_exit() {
		return true;
	}

	void on_pwd (const din::EntryPath& parDirMan) {
		std::cout << parDirMan.to_string() << '\n';
	}

	void on_ls (const din::ListDirContent& parLS, const din::EntryPath& parDirMan) {
		const auto& ls_result = parLS.ls(parDirMan);
		boost::copy(ls_result, std::ostream_iterator<std::string>(std::cout, "\n"));
	}

	void do_navigation (dindb::Backend& parDB) {
		const std::string prompt;
		din::ListDirContent ls(&parDB);
		din::LineReader lines(&ls);

		bool running = true;
		std::string curr_line;
		din::CommandProcessor proc;
		din::EntryPath dir_man;
		proc.add_command("exit", &on_exit, 0);
		proc.add_command("cd", std::function<void(const std::string&)>(std::bind(&din::EntryPath::push_piece, &dir_man, std::placeholders::_1)), 1);
		proc.add_command("disconnect", std::function<void()>(std::bind(&dindb::Backend::disconnect, std::ref(parDB))), 0);
		proc.add_command("pwd", std::function<void()>(std::bind(&on_pwd, std::ref(dir_man))), 0);
		proc.add_command("ls", std::function<void()>(std::bind(on_ls, std::ref(ls), std::ref(dir_man))), 0);
		do {
			do {
				curr_line = lines.read(prompt, dir_man.to_string());
			} while (curr_line.empty());
			running = proc.exec_command(curr_line);
		} while (running);

		parDB.disconnect();
	}
} //unnamed namespace
