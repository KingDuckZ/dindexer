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

#include <gtest/gtest.h>
#include "pathname.hpp"

TEST(machinery, pathname) {
	using mchlib::PathName;

	{
		PathName empty_path("");
		EXPECT_EQ("", empty_path.path());
		EXPECT_EQ(0, empty_path.atom_count());
	}

	{
		PathName test_slash("home/");
		EXPECT_FALSE(test_slash.is_absolute());
		EXPECT_EQ("home", test_slash.path());
		EXPECT_EQ(1, test_slash.atom_count());

		PathName test("home");
		EXPECT_FALSE(test.is_absolute());
		EXPECT_EQ("home", test.path());
		EXPECT_EQ(1, test.atom_count());

		EXPECT_EQ(test, test_slash);
	}

	{
		PathName test("/home/");
		EXPECT_TRUE(test.is_absolute());
		EXPECT_EQ("/home", test.path());
		EXPECT_EQ(1, test.atom_count());

		test.join("duckz/documents/libreoffice");
		EXPECT_TRUE(test.is_absolute());
		EXPECT_EQ(4, test.atom_count());
		EXPECT_EQ("/home/duckz/documents/libreoffice", test.path());

		test.pop_right();
		EXPECT_EQ(3, test.atom_count());
		EXPECT_EQ("/home/duckz/documents", test.path());

		test.join("attachments");
		test.join("important");
		EXPECT_EQ(5, test.atom_count());
		EXPECT_EQ("/home/duckz/documents/attachments/important", test.path());

		PathName attachments = mchlib::make_relative_path(PathName("/home/duckz/documents"), test);
		EXPECT_FALSE(attachments.is_absolute());
		EXPECT_EQ(2, attachments.atom_count());
		EXPECT_EQ("attachments/important", attachments.path());
	}
}

TEST(machinery, pathname_functions) {
	using mchlib::PathName;
	using mchlib::make_relative_path;
	using mchlib::basename;
	using mchlib::is_ancestor;
	using mchlib::are_siblings;
}
