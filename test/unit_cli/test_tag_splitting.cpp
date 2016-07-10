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
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "dindexer-core/split_tags.hpp"

TEST(cli_main, split_tags) {
	using dincore::split_tags;

	{
		const std::string test_tags = "tag1,tag2";
		const auto tags = split_tags(test_tags);
		EXPECT_EQ(static_cast<size_t>(2), tags.size());
		ASSERT_GE(static_cast<size_t>(2), tags.size());
		EXPECT_EQ("tag1", tags[0]);
		EXPECT_EQ("tag2", tags[1]);
	}

	{
		const std::string test_tags = "         ";
		const auto tags = split_tags(test_tags);
		EXPECT_TRUE(tags.empty());
	}

	{
		const std::string test_tags;
		const auto tags = split_tags(test_tags);
		EXPECT_TRUE(tags.empty());
	}

	{
		const std::string test_tags = ",,,,,,          ,,,, , ,  , ";
		const auto tags = split_tags(test_tags);
		EXPECT_TRUE(tags.empty());
	}

	{
		const std::string test_tags = ",,,a,  , ";
		const auto tags = split_tags(test_tags);
		EXPECT_EQ(static_cast<size_t>(1), tags.size());
		ASSERT_GE(static_cast<size_t>(1), tags.size());
		EXPECT_EQ("a", tags[0]);
	}

	{
		const std::string test_tags = ",,,test1,,,DuckZ ,  King_DuckZ    , ";
		const auto tags = split_tags(test_tags);
		EXPECT_EQ(static_cast<size_t>(3), tags.size());
		ASSERT_GE(static_cast<size_t>(3), tags.size());
		EXPECT_EQ("test1", tags[0]);
		EXPECT_EQ("DuckZ", tags[1]);
		EXPECT_EQ("King_DuckZ", tags[2]);
	}

	{
		const std::string test_tags = "test1";
		const auto tags = split_tags(test_tags);
		EXPECT_EQ(static_cast<size_t>(1), tags.size());
		ASSERT_GE(static_cast<size_t>(1), tags.size());
		EXPECT_EQ("test1", tags[0]);
	}

	{
		const std::string test_tags = "    test1";
		const auto tags = split_tags(test_tags);
		EXPECT_EQ(static_cast<size_t>(1), tags.size());
		ASSERT_GE(static_cast<size_t>(1), tags.size());
		EXPECT_EQ("test1", tags[0]);
	}

	{
		const std::string test_tags = "test1     ";
		const auto tags = split_tags(test_tags);
		EXPECT_EQ(static_cast<size_t>(1), tags.size());
		ASSERT_GE(static_cast<size_t>(1), tags.size());
		EXPECT_EQ("test1", tags[0]);
	}

	{
		const int count = 1500;
		std::ostringstream oss;
		for (int z = 0; z < count; ++z) {
			oss << z << " , ";
		}
		const std::string data = oss.str();
		const auto tags = split_tags(data);
		EXPECT_EQ(static_cast<size_t>(count), tags.size());
		ASSERT_GE(static_cast<size_t>(count), tags.size());
		for (int z = 0; z < count; ++z) {
			EXPECT_FALSE(tags[z].empty());
			EXPECT_EQ(boost::lexical_cast<std::string>(z), tags[z]);
		}
	}
}
