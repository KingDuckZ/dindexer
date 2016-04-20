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

/*
 *  Bits  Pattern
 *  ----  -------
 *    7   0xxxxxxx
 *   11   110xxxxx 10xxxxxx
 *   16   1110xxxx 10xxxxxx 10xxxxxx
 *   21   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *   26   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 *   32   111111xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 */

#include "utf8_ops.h"
#include <iso646.h>
#include <assert.h>

static size_t sequence_length ( char parChar ) a_pure;

/* See: http://www.daemonology.net/blog/2008-06-05-faster-utf8-strlen.html */
size_t utf8_strlen (const char* parString) {
	size_t i = 0;
	size_t i_before = 0;
	size_t count = 0;

	while ((signed char)parString[i] > 0) {
ascii:
		i++;
	}

	count += i - i_before;

	while (parString[i]) {
		if ((signed char)parString[i] > 0) {
			i_before = i;
			goto ascii;
		}
		else {
			i += sequence_length(parString[i]);
		}

		count++;
	}
	return count;
}

Character utf8_advance (const char** parString, const char* parStringEnd) {
	const Character masks[6] = {0x7f, 0x1f, 0x0f, 0x07, 0x03, 0x03};
	Character retval;
	uint8_t curr_code;
	int seq_len;
	int z;

	if (*parString >= parStringEnd) {
		*parString = parStringEnd;
		return 0;
	}

	curr_code = (uint8_t)(**parString);
	seq_len = sequence_length(curr_code);
	if (not seq_len) {
		++(*parString);
		return 0;
	}
	retval = curr_code bitand masks[seq_len - 1];

	for (z = 0; ++(*parString) < parStringEnd and z < (seq_len - 1); ++z) {
		curr_code = **parString;
		if ((curr_code bitand 0xc0) != 0x80)
			return 0;
		retval = (retval << 6) bitor (curr_code bitand 0x3f);
	}
	return retval;
}

size_t sequence_length (char parChar) {
	const uint8_t curr_code = (uint8_t)parChar;
	if (curr_code < 0x80) {
		return 1;
	}
	else if ((curr_code bitand 0xe0) == 0xc0) {
		return 2;
	}
	else if ((curr_code bitand 0xf0) == 0xe0) {
		return 3;
	}
	else if ((curr_code bitand 0xf8) == 0xf0) {
		return 4;
	}
	else if ((curr_code bitand 0xfc) == 0xf8) {
		return 5;
	}
	else if ((curr_code bitand 0xfc) == 0xfc) {
		return 6;
	}
	else {
		assert(0);
		return 0;
	}
}
