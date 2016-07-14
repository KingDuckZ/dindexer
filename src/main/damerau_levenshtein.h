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

#ifndef id88738025C6B24BDEB604A5AE3C36EE8D
#define id88738025C6B24BDEB604A5AE3C36EE8D

#include "duckhandy/compatibility.h"
#if defined(__cplusplus)
#	include <cstddef>
#else
#	include <stddef.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

int damerau_levenshtein (
	const char* parSource,
	const char* parTarget,
	int parDeleteCost,
	int parInsertCost,
	int parReplaceCost,
	int parSwapCost
) a_pure;

int damerau_levenshtein_with_size (
	const char* parSource,
	size_t parSourceLen,
	const char* parTarget,
	size_t parTargetLen,
	int parDeleteCost,
	int parInsertCost,
	int parReplaceCost,
	int parSwapCost
) a_pure;

#if defined(__cplusplus)
}
#endif

#endif
