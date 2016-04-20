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
#include "main/damerau_levenshtein.h"

TEST(cli_main, damerau_levenshtein) {
	{
		const char source1[] = u8"navigte";
		const char source2[] = u8"navigäte";
		const char source3[] = u8"navigbte";
		const char source4[] = u8"narigate";
		const char source5[] = u8"navigate";
		const char target[] = u8"navigate";
		EXPECT_EQ(1, damerau_levenshtein(source1, target, 1, 1, 1, 1));
		EXPECT_EQ(1, damerau_levenshtein(source2, target, 1, 1, 1, 1));
		EXPECT_EQ(1, damerau_levenshtein(source3, target, 1, 1, 1, 1));
		EXPECT_EQ(1, damerau_levenshtein(source4, target, 1, 1, 1, 1));
		EXPECT_EQ(0, damerau_levenshtein(source5, target, 1, 1, 1, 1));
	}
	{
		const char source[] = u8"navigaäte";
		const char target[] = u8"navigate";
		EXPECT_EQ(1, damerau_levenshtein(source, target, 1, 1, 1, 1));
	}
	{
		const char source1[] = u8"navigaäbcde";
		const char source2[] = u8"navigaabcde";
		const char target[] = u8"navigate";
		EXPECT_EQ(4, damerau_levenshtein(source1, target, 1, 1, 1, 1));
		EXPECT_EQ(4, damerau_levenshtein(source2, target, 1, 1, 1, 1));
	}
	{
		const char source[] = u8"";
		const char target[] = u8"navigate";
		EXPECT_EQ(8, damerau_levenshtein(source, target, 1, 1, 1, 1));
		EXPECT_EQ(8, damerau_levenshtein(target, source, 1, 1, 1, 1));
	}
	{
		const char source[] = u8"ありがとうございました";
		const char target[] = u8"りがとうございました";
		EXPECT_EQ(1, damerau_levenshtein(source, target, 1, 1, 1, 1));
	}
	{
		const char source1[] = u8"Kanji 金 reads かね.";
		const char source2[] = u8"Kanji ⿔ reads かめ.";
		const char target[] = u8"Kanji 亀 reads かめ.";
		EXPECT_EQ(2, damerau_levenshtein(source1, target, 1, 1, 1, 1));
		EXPECT_EQ(1, damerau_levenshtein(source2, target, 1, 1, 1, 1));
	}
	{
		const char source[] = u8"\"槪\" and \"🚮\" have a long utf8 sequence.";
		const char target1[] = u8"\"槪\" or \"🚮\" have a long utf8 sequence.";
		const char target2[] = u8"\"槪\" or \"🚻\" have a long utf8 sequence.";
		EXPECT_EQ(3, damerau_levenshtein(source, target1, 1, 1, 1, 1));
		EXPECT_EQ(4, damerau_levenshtein(source, target2, 1, 1, 1, 1));
	}
}
