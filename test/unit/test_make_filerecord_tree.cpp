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

#include "dindexer-machinery/make_filerecord_tree.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(machinery, make_filerecord_tree) {
	using mchlib::make_filerecord_tree;
	using mchlib::FileRecordNode;
	using mchlib::FileRecordData;

	std::vector<FileRecordData> records {
		{"unit/CMakeFiles/dindexer-test.dir/test_guess_content_type.cpp.o", 0, 0, 0, 4, false, false},
		{"unit/CMakeFiles/dindexer-test.dir/build.make", 0, 0, 0, 4, false, false},
		{"unit_cli/CMakeFiles", 0, 0, 0, 2, true, false},
		{"unit_cli/CMakeFiles/dindexer-test_cli.dir/C.includecache", 0, 0, 0, 4, false, false},
		{"unit/CMakeFiles", 0, 0, 0, 2, true, false},
		{"unit_cli/cmake_install.cmake", 0, 0, 0, 2, false, false},
		{"unit_cli/CTestTestfile.cmake", 0, 0, 0, 2, false, false},
		{"gtest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o", 0, 0, 0, 4, false, false},
		{"gtest/CMakeFiles/gtest_main.dir/link.txt", 0, 0, 0, 4, false, false},
		{"gtest/CMakeFiles/CMakeDirectoryInformation.cmake", 0, 0, 0, 4, false, false},
		{"unit_cli/Makefile", 0, 0, 0, 2, false, false},
		{"gtest/CMakeFiles/progress.marks", 0, 0, 0, 3, false, false},
		{"gtest/CMakeFiles/gtest.dir", 0, 0, 0, 3, true, false},
		{"unit/Makefile", 0, 0, 0, 2, false, false},
		{"unit/cmake_install.cmake", 0, 0, 0, 2, false, false},
		{"unit/dindexer-test", 0, 0, 0, 2, false, false},
		{"unit_cli/CMakeFiles/CMakeDirectoryInformation.cmake", 0, 0, 0, 3, false, false},
		{"unit_cli/CMakeFiles/progress.marks", 0, 0, 0, 3, false, false},
		{"gtest", 0, 0, 0, 1, true, false},
		{"gtest/CMakeFiles/gtest_main.dir/depend.internal", 0, 0, 0, 4, false, false},
		{"gtest/CMakeFiles/gtest_main.dir/src", 0, 0, 0, 4, true, false},
		{"unit/CTestTestfile.cmake", 0, 0, 0, 2, false, false},
		{"unit/CMakeFiles/CMakeDirectoryInformation.cmake", 0, 0, 0, 3, false, false},
		{"unit/CMakeFiles/progress.marks", 0, 0, 0, 3, false, false},
		{"gtest/CMakeFiles/gtest.dir/cmake_clean_target.cmake", 0, 0, 0, 4, false, false},
		{"gtest/CMakeFiles/gtest.dir/build.make", 0, 0, 0, 4, false, false},
		{"gtest/CMakeFiles/gtest.dir/cmake_clean.cmake", 0, 0, 0, 4, false, false},
		{"unit", 0, 0, 0, 1, true, false}
	};
	std::vector<FileRecordNode> results = make_filerecord_tree(records);

	for (const auto& result : results) {
		std::cout << result.index << ' ' << records[result.index].path() << '\n';
	}
}
