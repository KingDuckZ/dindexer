/* Copyright (c) 2012 Kevin L. Stern
 * Copyright (c) 2016 Michele Santullo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


/*
 * See
 * http://software-and-algorithms.blogspot.ca/2012/09/damerau-levenshtein-edit-distance.html
 * and
 * https://github.com/KevinStern/software-and-algorithms/blob/master/src/main/java/blogspot/software_and_algorithms/stern_library/string/DamerauLevenshteinAlgorithm.java
 */


/**
 * The Damerau-Levenshtein Algorithm is an extension to the Levenshtein
 * Algorithm which solves the edit distance problem between a source string and
 * a target string with the following operations:
 *
 * <ul>
 * <li>Character Insertion</li>
 * <li>Character Deletion</li>
 * <li>Character Replacement</li>
 * <li>Adjacent Character Swap</li>
 * </ul>
 *
 * Note that the adjacent character swap operation is an edit that may be
 * applied when two adjacent characters in the source string match two adjacent
 * characters in the target string, but in reverse order, rather than a general
 * allowance for adjacent character swaps.
 * <p>
 *
 * This implementation allows the client to specify the costs of the various
 * edit operations with the restriction that the cost of two swap operations
 * must not be less than the cost of a delete operation followed by an insert
 * operation. This restriction is required to preclude two swaps involving the
 * same character being required for optimality which, in turn, enables a fast
 * dynamic programming solution.
 * <p>
 *
 * The running time of the Damerau-Levenshtein algorithm is O(n*m) where n is
 * the length of the source string and m is the length of the target string.
 * This implementation consumes O(n*m) space.
 *
 * @author Kevin L. Stern
 */

#include "damerau_levenshtein.h"
#include "pbl_wrapper.h"
#include "utf8_ops.h"
#include <string.h>
#include <iso646.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>

//See: http://stackoverflow.com/questions/3437404/min-and-max-in-c
#define min(a,b) \
	({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a < _b ? _a : _b; })
#define max(a,b) \
	({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a > _b ? _a : _b; })

static void insert_pair (PblMap* parMap, Character parKey, int parValue) {
	const int retval = pblMapAdd(
		parMap,
		&parKey,
		sizeof(parKey),
		&parValue,
		sizeof(parValue)
	);
	assert(0 <= retval);
}

static int get_value (PblMap* parMap, Character parKey) {
	size_t ret_len;
	void* value = pblMapGet(parMap, &parKey, sizeof(parKey), &ret_len);
	assert(not value or (sizeof(int) == ret_len));
	return (value ? *(int*)value : -1);
}

int damerau_levenshtein (
	const char* parSource,
	const char* parTarget,
	int parDeleteCost,
	int parInsertCost,
	int parReplaceCost,
	int parSwapCost
)
{
	return damerau_levenshtein_with_size(
		parSource,
		utf8_strlen(parSource),
		parTarget,
		utf8_strlen(parTarget),
		parDeleteCost,
		parInsertCost,
		parReplaceCost,
		parSwapCost
	);
}

/**
 * Compute the Damerau-Levenshtein distance between the specified source
 * string and the specified target string.
 */
int damerau_levenshtein_with_size (
	const char* parSource,
	size_t parSourceLen,
	const char* parTarget,
	size_t parTargetLen,
	int parDeleteCost,
	int parInsertCost,
	int parReplaceCost,
	int parSwapCost
)
{
	int i;
	int j;
	int* table;
	PblMap* sourceIndexByCharacter;
	int delete_distance;
	int insert_distance;
	int match_distance;
	int swap_distance;
	int maxSourceLetterMatchIndex;
	int candidateSwapIndex;
	int i_swap;
	int j_swap;
	int pre_swap_cost;
	int retval;
	const char* source;
	const char* target;
	const char* source_index_1;
	const char* target_index_1;
	const char* source_end = parSource + strlen(parSource);
	const char* target_end = parTarget + strlen(parTarget);
	Character source_char;
	Character target_char;
	Character source_char_0;
	Character target_char_0;

	assert(parSource);
	assert(parTarget);

    /*
     * Required to facilitate the premise to the algorithm that two swaps of the
     * same character are never required for optimality.
     */
	if (2 * parSwapCost < parInsertCost + parDeleteCost) {
		/*throw new IllegalArgumentException("Unsupported cost assignment");*/
		return -1;
	}

	if (0 == parSourceLen)
		return parTargetLen * parInsertCost;
	if (0 == parTargetLen)
		return parSourceLen * parDeleteCost;

	const int table_length = parSourceLen * parTargetLen;
	table = malloc(sizeof(int) * table_length);
	memset(table, 0, sizeof(int) * table_length);

	sourceIndexByCharacter = pblMapNewHashMap();
	assert(sourceIndexByCharacter);

	source = parSource;
	target = parTarget;
	source_char_0 = utf8_advance(&source, source_end);
	target_char_0 = utf8_advance(&target, target_end);
	source_index_1 = source;
	target_index_1 = target;
	if (source_char_0 != target_char_0) {
		table[0 /*source*/ + 0 /*target*/ * parSourceLen] =
			min(parReplaceCost, parDeleteCost + parInsertCost);
	}
	insert_pair(sourceIndexByCharacter, source_char_0, 0);

	assert(source = source_index_1);
	for (i = 1; i < parSourceLen; ++i) {
		source_char = utf8_advance(&source, source_end);
		delete_distance = table[i - 1 + 0 * parSourceLen] + parDeleteCost;
		insert_distance = (i + 1) * parDeleteCost + parInsertCost;
		match_distance = i * parDeleteCost +
			(source_char == target_char_0 ? 0 : parReplaceCost);
		table[i + 0 * parSourceLen] = min(
			min(delete_distance, insert_distance), match_distance
		);
	}

	assert(target == target_index_1);
	for (j = 1; j < parTargetLen; ++j) {
		target_char = utf8_advance(&target, target_end);
		delete_distance = (j + 1) * parInsertCost + parDeleteCost;
		insert_distance = table[0 + (j - 1) * parSourceLen] + parInsertCost;
		match_distance = j * parInsertCost +
			(source_char_0 == target_char ? 0 : parReplaceCost);
		table[0 + j * parSourceLen] = min(
			min(delete_distance, insert_distance), match_distance
		);
	}

	source = source_index_1;
	for (i = 1; i < parSourceLen; ++i) {
		source_char = utf8_advance(&source, source_end);
		maxSourceLetterMatchIndex = (source_char == target_char_0 ? 0 : -1);
		target = target_index_1;
		for (j = 1; j < parTargetLen; ++j) {
			target_char = utf8_advance(&target, target_end);
			candidateSwapIndex = get_value(sourceIndexByCharacter, target_char);
			j_swap = maxSourceLetterMatchIndex;
			delete_distance = table[(i - 1) + j * parSourceLen] + parDeleteCost;
			insert_distance = table[i + (j - 1) * parSourceLen] + parInsertCost;
			match_distance = table[(i - 1) + (j - 1) * parSourceLen];
			if (source_char != target_char)
				match_distance += parReplaceCost;
			else
				maxSourceLetterMatchIndex = j;

			if (-1 != candidateSwapIndex and -1 != j_swap) {
				i_swap = candidateSwapIndex;
				if (0 == i_swap and 0 == j_swap)
					pre_swap_cost = 0;
				else
					pre_swap_cost = table[
						max(0, i_swap - 1) + max(0, j_swap - 1) * parSourceLen
					];
				swap_distance = pre_swap_cost + (i - i_swap - 1) *
					parDeleteCost + (j - j_swap - 1) * parInsertCost +
					parSwapCost;
			}
			else {
				swap_distance = INT_MAX;
			}
			table[i + j * parSourceLen] = min(
				min(
					min(delete_distance, insert_distance),
					match_distance
				),
				swap_distance
			);
		}
		insert_pair(sourceIndexByCharacter, source_char, i);
	}

	retval = table[(parSourceLen - 1) + (parTargetLen - 1) * parSourceLen];
	free(table);
	pblMapFree(sourceIndexByCharacter);
	return retval;
}
