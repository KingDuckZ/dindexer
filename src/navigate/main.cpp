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
#include "dindexer-common/settings.hpp"
#include "genericpath.hpp"
#include "dbsource.hpp"
#include "dindexerConfig.h"
#include "helpers/infix_iterator.hpp"
#include <iostream>
#include <ciso646>
#include <string>
#include <vector>
#include <cassert>
#include <boost/range/algorithm/copy.hpp>
#include <boost/lexical_cast.hpp>

namespace {
	void do_navigation ( din::DBSource& parDB );

	template <typename V> void print_db_result ( const V& parResult );
	bool on_exit ( void );
	void on_pwd ( const din::GenericPath& parDirMan );
	void on_ls ( const din::GenericPath& parDirMan, din::DBSource& parDB );
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
	{
		const bool loaded = dinlib::load_settings(CONFIG_FILE_PATH, settings);
		if (not loaded) {
			std::cerr << "Can't load settings from " << CONFIG_FILE_PATH << ", quitting\n";
			return 1;
		}
	}

	din::DBSource db_source(settings.db);

	do_navigation(db_source);
	return 0;
}

namespace {
	template <typename V>
	void print_db_result (const V& parResult) {
		for (const auto& row : parResult) {
			boost::copy(row, infix_ostream_iterator<std::string>(std::cout, "\t"));
			std::cout << "\n";
		}
	}

	bool on_exit() {
		return true;
	}

	void on_pwd (const din::GenericPath& parDirMan) {
		std::cout << parDirMan.to_string() << '\n';
	}

	void on_ls (const din::GenericPath& parDirMan, din::DBSource& parDB) {
		using namespace din;

		const auto curr_path = parDirMan.to_string();
		if ("/" == curr_path) {
			auto sets_ids = parDB.sets();
			auto sets_info = parDB.set_details<SetDetail_ID, SetDetail_Desc, SetDetail_CreeationDate>(sets_ids);
			print_db_result(sets_info);
		}
		else {
			const auto start_from = curr_path.find('/', 1);
			auto path_prefix = boost::string_ref(curr_path).substr(start_from == curr_path.npos ? curr_path.size() : start_from + 1);
			const auto set_id = boost::lexical_cast<uint32_t>(parDirMan[0]);
			auto files_info = parDB.file_details<FileDetail_Path>(set_id, parDirMan.level(), path_prefix);
			print_db_result(files_info);
		}
	}

	void do_navigation (din::DBSource& parDB) {
		auto& inp = std::cin;

		bool running = true;
		std::string curr_line;
		din::CommandProcessor proc;
		din::GenericPath dir_man;
		proc.add_command("exit", &on_exit, 0);
		proc.add_command("cd", std::function<void(const std::string&)>(std::bind(&din::GenericPath::push_piece, &dir_man, std::placeholders::_1)), 1);
		proc.add_command("disconnect", std::function<void()>(std::bind(&din::DBSource::disconnect, &parDB)), 0);
		proc.add_command("pwd", std::function<void()>(std::bind(&on_pwd, std::ref(dir_man))), 0);
		proc.add_command("ls", std::function<void()>(std::bind(&on_ls, std::ref(dir_man), std::ref(parDB))), 0);
		do {
			do {
				std::getline(inp, curr_line);
			} while (curr_line.empty());
			running = proc.exec_command(curr_line);
		} while (running);

		parDB.disconnect();
	}
} //unnamed namespace
