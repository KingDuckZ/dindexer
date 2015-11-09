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

int main (int parArgc, char* parArgv[]) {
	using std::placeholders::_1;
	using std::placeholders::_2;
	using std::placeholders::_3;
	using std::placeholders::_4;
	std::cout << "argc: " << parArgc << '\n';
	for (int z = 0; z < parArgc; ++z) {
		std::cout << "argv[" << z << "]: " << parArgv[z] << '\n';
	}
	std::cout << std::endl;

	din::Indexer indexer;
	fastf::FileSearcher searcher("/home/michele/dev/code/cpp/dindexer/test");
	fastf::FileSearcher::ConstCharVecType ext, ignore;
	searcher.SetFollowSymlinks(true);
	searcher.SetCallback(fastf::FileSearcher::CallbackType(std::bind(&din::Indexer::add_path, &indexer, _1, _2, _3, _4)));
	searcher.Search(ext, ignore);

	indexer.calculate_hash();
	indexer.dump();
	return 0;
}
