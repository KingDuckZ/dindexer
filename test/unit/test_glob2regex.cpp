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

#include "glob2regex/glob2regex.hpp"
#include <gtest/gtest.h>
#include <string>

TEST(glob2regex, convert) {
	{
		const std::string glob = "*.ogg";
		const std::string expected_regex = "[^/]*\\.ogg$";
		const auto auto_regex = g2r::convert(glob);
		EXPECT_EQ(expected_regex, auto_regex);
	}

	{
		const std::string glob = "*.jp*g";
		const std::string expected_regex = "[^/]*\\.jp[^/]*g$";
		const auto auto_regex = g2r::convert(glob);
		EXPECT_EQ(expected_regex, auto_regex);
	}

	{
		const std::string glob = "/home/duckz/sample_file.bin";
		const std::string expected_regex = "/home/duckz/sample_file\\.bin$";
		const auto auto_regex = g2r::convert(glob);
		EXPECT_EQ(expected_regex, auto_regex);
	}

	{
		const std::string glob = "**/main.{c,h}pp";
		const std::string expected_regex = ".*/main\\.[ch]pp$";
		const auto auto_regex = g2r::convert(glob);
		EXPECT_EQ(expected_regex, auto_regex);
	}

	{
		const std::string glob = "photo_???.JPG";
		const std::string expected_regex = "photo_[^/][^/][^/]\\.JPG$";
		const auto auto_regex = g2r::convert(glob);
		EXPECT_EQ(expected_regex, auto_regex);
	}

	{
		const std::string glob = "nested_{example,*sample,test}";
		const std::string expected_regex = "nested_(?:example|[^/]*sample|test)$";
		const auto auto_regex = g2r::convert(glob);
		EXPECT_EQ(expected_regex, auto_regex);
	}

	{
		const std::string glob = "nested_{example,*sample,test";
		EXPECT_THROW(g2r::convert(glob), g2r::ParsingError);
	}

	{
		const std::string glob = "]";
		EXPECT_THROW(g2r::convert(glob), g2r::ParsingError);
	}
}
