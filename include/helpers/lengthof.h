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

#ifndef id19B690A53A9546D5BD95D89FFF388283
#define id19B690A53A9546D5BD95D89FFF388283

#if defined(__cplusplus)
#	include <cstddef>
#else
#	include <stddef.h>
#endif

#if defined(lengthof)
#	undef lengthof
#endif
//http://stackoverflow.com/questions/4415524/common-array-length-macro-for-c#4415646
#if defined(__cplusplus)
#	define lengthof(x) ((sizeof(x)/sizeof(0[x])) / ((std::size_t)(!(sizeof(x) % sizeof(0[x])))))
#else
#	define lengthof(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#endif

#endif
