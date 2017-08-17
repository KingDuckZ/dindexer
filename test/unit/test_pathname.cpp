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

	{
		PathName test("/////home/duckz/////many////slashes///////////////");
		EXPECT_EQ(4, test.atom_count());
		EXPECT_TRUE(test.is_absolute());
		EXPECT_EQ("/home/duckz/many/slashes", test.path());

		test.join("..");
		EXPECT_EQ(5, test.atom_count());
		EXPECT_EQ("/home/duckz/many/slashes/..", test.path());

		test.pop_right();
		EXPECT_EQ(4, test.atom_count());
		EXPECT_EQ("/home/duckz/many/slashes", test.path());
		test.pop_right();
		EXPECT_EQ(3, test.atom_count());
		EXPECT_EQ("/home/duckz/many", test.path());
		test.pop_right();
		EXPECT_EQ(2, test.atom_count());
		EXPECT_EQ("/home/duckz", test.path());
		test.pop_right();
		EXPECT_EQ(1, test.atom_count());
		EXPECT_EQ("/home", test.path());
		test.pop_right();
		EXPECT_EQ(0, test.atom_count());
		EXPECT_EQ("/", test.path());
	}
}

TEST(machinery, pathname_functions) {
	using mchlib::PathName;
	//using mchlib::make_relative_path;
	using mchlib::basename;
	using mchlib::is_ancestor;
	using mchlib::are_siblings;

	PathName home("/home/duckz/");
	PathName jpeg("pic.jpg");
	PathName bpg("pic2.pbg");
	PathName pics_rel("pictures/wallpapers");
	PathName full_jpeg_path(home);
	full_jpeg_path.join(pics_rel);
	full_jpeg_path.join(jpeg);
	PathName full_bpg_path(home);
	full_bpg_path.join(pics_rel);
	full_bpg_path.join(bpg);
	PathName pics_abs(home);
	pics_abs.join(pics_rel);

	EXPECT_EQ(2, home.atom_count());
	EXPECT_EQ(1, jpeg.atom_count());
	EXPECT_EQ(2, pics_rel.atom_count());
	EXPECT_EQ(4, pics_abs.atom_count());

	EXPECT_EQ("/home/duckz/pictures/wallpapers", pics_abs.path());
	EXPECT_EQ("/home/duckz/pictures/wallpapers/pic.jpg", full_jpeg_path.path());

	EXPECT_EQ("pic.jpg", basename(full_jpeg_path));
	EXPECT_EQ("pic.jpg", basename(jpeg));
	EXPECT_EQ("wallpapers", basename(pics_abs));
	EXPECT_EQ("wallpapers", basename(pics_rel));

	EXPECT_FALSE(are_siblings(home, pics_rel));
	EXPECT_FALSE(are_siblings(home, pics_abs));
	EXPECT_TRUE(are_siblings(full_bpg_path, full_jpeg_path));
	EXPECT_TRUE(are_siblings(bpg, jpeg));

	EXPECT_TRUE(is_ancestor(home, pics_abs, 0));
	EXPECT_FALSE(is_ancestor(home, pics_abs, 1));
	EXPECT_TRUE(is_ancestor(home, pics_abs, 2));
	EXPECT_TRUE(is_ancestor(home, pics_abs, 3));
}
