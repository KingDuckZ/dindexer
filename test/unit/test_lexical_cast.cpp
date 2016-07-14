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

#include "duckhandy/lexical_cast.hpp"
#include <gtest/gtest.h>
#include <cstdint>
#include <string>
#include <boost/utility/string_ref.hpp>

TEST(duckhandy, lexical_cast) {
	using dinhelp::lexical_cast;
	using dinhelp::tags::hex;
	using dinhelp::tags::dec;
	using dinhelp::tags::bin;

	{
		const uint8_t value = 0x34;
		const auto hex_str = lexical_cast<std::string, hex>(value);
		EXPECT_EQ("34", hex_str);
		const auto dec_str = lexical_cast<std::string, dec>(value);
		EXPECT_EQ("52", dec_str);
		const auto bin_str = lexical_cast<std::string, bin>(value);
		EXPECT_EQ("110100", bin_str);
	}
	{
		const uint16_t value = 0xFFFF;
		const auto hex_str = lexical_cast<std::string, hex>(value);
		EXPECT_EQ("FFFF", hex_str);
		const auto dec_str = lexical_cast<std::string, dec>(value);
		EXPECT_EQ("65535", dec_str);
		const auto bin_str = lexical_cast<std::string, bin>(value);
		EXPECT_EQ("1111111111111111", bin_str);
	}
	{
		const uint16_t value = 0xFF;
		const auto hex_str = lexical_cast<std::string, hex>(value);
		EXPECT_EQ("FF", hex_str);
		const auto dec_str = lexical_cast<std::string, dec>(value);
		EXPECT_EQ("255", dec_str);
		const auto bin_str = lexical_cast<std::string, bin>(value);
		EXPECT_EQ("11111111", bin_str);
	}
	{
		const uint16_t value = 0x100;
		const auto hex_str = lexical_cast<std::string, hex>(value);
		EXPECT_EQ("100", hex_str);
		const auto dec_str = lexical_cast<std::string, dec>(value);
		EXPECT_EQ("256", dec_str);
		const auto bin_str = lexical_cast<std::string, bin>(value);
		EXPECT_EQ("100000000", bin_str);
	}
	{
		const uint32_t value = 0x1ABC;
		const auto hex_str = lexical_cast<std::string, hex>(value);
		EXPECT_EQ("1ABC", hex_str);
		const auto dec_str = lexical_cast<std::string, dec>(value);
		EXPECT_EQ("6844", dec_str);
		const auto bin_str = lexical_cast<std::string, bin>(value);
		EXPECT_EQ("1101010111100", bin_str);
	}
	{
		const uint32_t value = 0xffffffff;
		const auto hex_str = lexical_cast<std::string, hex>(value);
		EXPECT_EQ("FFFFFFFF", hex_str);
		const auto dec_str = lexical_cast<std::string, dec>(value);
		EXPECT_EQ("4294967295", dec_str);
		const auto bin_str = lexical_cast<std::string, bin>(value);
		EXPECT_EQ("11111111111111111111111111111111", bin_str);
	}
	{
		const uint32_t value = 1;
		const auto hex_str = lexical_cast<std::string, hex>(value);
		EXPECT_EQ("1", hex_str);
		const auto dec_str = lexical_cast<std::string, dec>(value);
		EXPECT_EQ("1", dec_str);
		const auto bin_str = lexical_cast<std::string, bin>(value);
		EXPECT_EQ("1", bin_str);
	}
	{
		const uint32_t value = 0;
		const auto hex_str = lexical_cast<std::string, hex>(value);
		EXPECT_EQ("0", hex_str);
		const auto dec_str = lexical_cast<std::string, dec>(value);
		EXPECT_EQ("0", dec_str);
		const auto bin_str = lexical_cast<std::string, bin>(value);
		EXPECT_EQ("0", bin_str);
	}
	{
		const uint64_t value = 0x12abcd34;
		const auto hex_str = lexical_cast<std::string, hex>(value);
		EXPECT_EQ("12ABCD34", hex_str);
		const auto dec_str = lexical_cast<std::string, dec>(value);
		EXPECT_EQ("313249076", dec_str);
		const auto bin_str = lexical_cast<std::string, bin>(value);
		EXPECT_EQ("10010101010111100110100110100", bin_str);
	}
	{
		const int32_t value = 0x7800;
		const auto hex_str = lexical_cast<std::string, hex>(value);
		EXPECT_EQ("7800", hex_str);
		const auto dec_str = lexical_cast<std::string, dec>(value);
		EXPECT_EQ("30720", dec_str);
		const auto bin_str = lexical_cast<std::string, bin>(value);
		EXPECT_EQ("111100000000000", bin_str);
	}
	{
		const int32_t value = -1400;
		const auto hex_str = lexical_cast<std::string, hex>(value);
		EXPECT_EQ("FFFFFA88", hex_str);
		const auto dec_str = lexical_cast<std::string, dec>(value);
		EXPECT_EQ("-1400", dec_str);
		const auto bin_str = lexical_cast<std::string, bin>(value);
		EXPECT_EQ("11111111111111111111101010001000", bin_str);
	}
	{
		const int32_t value = 515;
		const auto hex_str = lexical_cast<std::string, hex>(value);
		EXPECT_EQ("203", hex_str);
		const auto dec_str = lexical_cast<std::string, dec>(value);
		EXPECT_EQ("515", dec_str);
		const auto bin_str = lexical_cast<std::string, bin>(value);
		EXPECT_EQ("1000000011", bin_str);
	}
	{
		const int64_t value = 515;
		const auto hex_str = lexical_cast<std::string, hex>(value);
		EXPECT_EQ("203", hex_str);
		const auto dec_str = lexical_cast<std::string, dec>(value);
		EXPECT_EQ("515", dec_str);
		const auto bin_str = lexical_cast<std::string, bin>(value);
		EXPECT_EQ("1000000011", bin_str);
	}
	{
		const uint32_t value = 515;
		const auto hex_str = lexical_cast<std::string, hex>(value);
		EXPECT_EQ("203", hex_str);
		const auto dec_str = lexical_cast<std::string, dec>(value);
		EXPECT_EQ("515", dec_str);
		const auto bin_str = lexical_cast<std::string, bin>(value);
		EXPECT_EQ("1000000011", bin_str);
	}
	{
		const uint64_t value = 515;
		const auto hex_str = lexical_cast<std::string, hex>(value);
		EXPECT_EQ("203", hex_str);
		const auto dec_str = lexical_cast<std::string, dec>(value);
		EXPECT_EQ("515", dec_str);
		const auto bin_str = lexical_cast<std::string, bin>(value);
		EXPECT_EQ("1000000011", bin_str);
	}
}
