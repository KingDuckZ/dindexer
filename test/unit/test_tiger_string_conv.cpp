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

#include "dindexer-machinery/tiger.hpp"
#include <gtest/gtest.h>
#include <string>

TEST(machinery, TigerStringConv) {
	using mchlib::TigerHash;

	const std::size_t TigerStringCharCount = sizeof(TigerHash) * 2;
	{
		TigerHash tiger_zero;
		tiger_zero.part_a = tiger_zero.part_b = tiger_zero.part_c = 0;

		const std::string expected(TigerStringCharCount, '0');
		const std::string got = mchlib::tiger_to_string(tiger_zero);
		EXPECT_EQ(expected, got);

		auto hash2 = mchlib::string_to_tiger(got);
		EXPECT_EQ(tiger_zero, hash2);
	}

	{
		TigerHash t;
		mchlib::tiger_init_hash(t);

		const std::string expected = "efcdab89674523011032547698badcfe87e1b2c3b4a596f0";
		EXPECT_EQ(TigerStringCharCount, expected.size());
		const std::string got = mchlib::tiger_to_string(t);
		EXPECT_EQ(expected, got);

		auto hash2 = mchlib::string_to_tiger(got);
		EXPECT_EQ(t, hash2);
	}

	{
		TigerHash t;
		t.part_a = t.part_b = t.part_c = 0xababababababababULL;

		const std::string expected = "abababababababababababababababababababababababab";
		EXPECT_EQ(TigerStringCharCount, expected.size());
		const std::string got = mchlib::tiger_to_string(t);
		EXPECT_EQ(expected, got);

		auto hash2 = mchlib::string_to_tiger(got);
		EXPECT_EQ(t, hash2);
	}
}
