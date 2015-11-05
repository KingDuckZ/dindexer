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
#include "pathname.hpp"

bool my_callback (const char* parPath, int parLevel, bool parDir, bool parSymlink) {
	if (parLevel > 0 and not parDir)
		std::cout << parPath << '\n';
	return true;
}

int main (int parArgc, char* parArgv[]) {
	std::cout << "argc: " << parArgc << '\n';
	for (int z = 0; z < parArgc; ++z) {
		std::cout << "argv[" << z << "]: " << parArgv[z] << '\n';
	}
	std::cout << std::endl;

	fastf::FileSearcher searcher("/home/duckz/dev/code/cpp/dindexer/test");
	fastf::FileSearcher::ConstCharVecType ext, ignore;
	searcher.SetFollowSymlinks(true);
	searcher.SetCallback(fastf::FileSearcher::CallbackType(&my_callback));
	//searcher.Search(ext, ignore);

	din::PathName pn("/home/duckz/dev/code/cpp/dindexer/test");
	din::PathName pn1("///home/duckz/dev/code/cpp/dindexer/test");
	din::PathName pn2("home/duckz////dev/code/cpp/dindexer/test");
	din::PathName pn3("/home/duckz/dev/code/cpp/dindexer/test/");
	din::PathName pn4("/home/duckz/dev/code/cpp/dindexer/test////");

	std::cout << pn.path() << '\n';
	std::cout << pn1.path() << '\n';
	std::cout << pn2.path() << '\n';
	std::cout << pn3.path() << '\n';
	std::cout << pn4.path() << '\n';

	pn1.join(pn2);
	std::cout << pn1.path() << '\n';
	return 0;
}
