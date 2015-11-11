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

#include <iostream>
#include <ciso646>
#include "filesearcher.hpp"
#include "indexer.hpp"
#include "settings.hpp"
#include "commandline.hpp"

int main (int parArgc, char* parArgv[]) {
	using std::placeholders::_1;
	using std::placeholders::_2;
	using std::placeholders::_3;
	using std::placeholders::_4;
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
	const std::string search_path(vm["search-path"].as<std::string>());

	din::DinDBSettings settings;
	{
		const bool loaded = din::load_settings("dindexerrc.yml", settings);
		if (not loaded) {
			std::cerr << "Can't load settings from dindexerrc.yml, quitting\n";
			return 1;
		}
	}

	din::Indexer indexer(settings);
	fastf::FileSearcher searcher(search_path);
	fastf::FileSearcher::ConstCharVecType ext, ignore;
	searcher.SetFollowSymlinks(true);
	searcher.SetCallback(fastf::FileSearcher::CallbackType(std::bind(&din::Indexer::add_path, &indexer, _1, _2, _3, _4)));
	searcher.Search(ext, ignore);

	if (indexer.empty()) {
		std::cerr << "Nothing found at the given location, quitting\n";
		return 1;
	}
	else {
		indexer.calculate_hash();
		indexer.add_to_db(vm["setname"].as<std::string>(), vm["type"].as<char>());
	}
	return 0;
}
