/* Copyright 2015, Michele Santullo
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

#include <string.h>
#include <stdio.h>
#include <iso646.h>
#include <unistd.h>
#include <stdlib.h>
#include "dindexerConfig.h"
#include "findactions.h"
#include "helpers/lengthof.h"

static size_t foreach_avail_action ( int(*parFunc)(const char*, const char*), char** parList, size_t parCount, char* parPass );
static int printf_stderr ( const char* parMsg, const char* parUnused );
static int same_action ( const char* parAction1, const char* parAction2 );

int main (int parArgc, char* parArgv[]) {
	size_t z;
	size_t actions_count;
	char** actions;
	char* action_path;
	size_t action_path_length;
	size_t selected_action;
	char** argv;

	find_actions(&actions, &actions_count);
	if (0 == actions_count) {
		fprintf(stderr, "No actions found in \"%s\"\n", ACTIONS_SEARCH_PATH);
		return 1;
	}

	if (parArgc < 2 or strcmp("-h", parArgv[1]) == 0 or strcmp("--help", parArgv[1]) == 0) {
		fprintf(stderr, "No action specified. Available actions are:\n");
		foreach_avail_action(&printf_stderr, actions, actions_count, NULL);
		free_actions(actions, actions_count);
		if (parArgc < 2) {
			return 2;
		}
		else {
			return 0;
		}
	}

	selected_action = foreach_avail_action(&same_action, actions, actions_count, parArgv[1]);

	if (actions_count == selected_action) {
		fprintf(stderr, "Unrecognized action \"%s\" - available actions are:\n", parArgv[1]);
		foreach_avail_action(&printf_stderr, actions, actions_count, NULL);
		free_actions(actions, actions_count);
		return 2;
	}

	action_path_length = lengthof(ACTIONS_SEARCH_PATH) - 1;
	if (ACTIONS_SEARCH_PATH[lengthof(ACTIONS_SEARCH_PATH) - 2] != '/') {
		++action_path_length;
	}
	action_path_length += strlen(actions[selected_action]);
	++action_path_length;

	action_path = (char*)malloc(action_path_length);
	memcpy(action_path, ACTIONS_SEARCH_PATH, lengthof(ACTIONS_SEARCH_PATH) - 1);
	z = lengthof(ACTIONS_SEARCH_PATH) - 1;
	if (ACTIONS_SEARCH_PATH[lengthof(ACTIONS_SEARCH_PATH) - 2] != '/') {
		action_path[lengthof(ACTIONS_SEARCH_PATH) - 1] = '/';
		++z;
	}
	strcpy(action_path + z, actions[selected_action]);

	free_actions(actions, actions_count);

	argv = (char**)malloc(sizeof(char*) * (parArgc - 1 + 1));
	argv[0] = action_path;
	for (z = 2; z <= parArgc; ++z) {
		argv[z - 1] = parArgv[z];
	}

	/*printf("would call %s\n", action_path);*/
	execv(action_path, argv);

	/* the program won't get here, but just to be clean... */
	free(action_path);
	return 0;
}

static size_t foreach_avail_action(int(*parFunc)(const char*, const char*), char** parList, size_t parCount, char* parPass) {
	size_t z;
	const char* cmd_name_start;
	int stop;

	for (z = 0; z < parCount; ++z) {
		cmd_name_start = get_actionname(parList[z]);

		stop = (*parFunc)(cmd_name_start, parPass);
		if (stop) {
			return z;
		}
	}
	return z;
}

static int printf_stderr (const char* parMsg, const char* parUnused) {
	fprintf(stderr, "\t%s\n", parMsg);
	return 0;
}

static int same_action (const char* parAction1, const char* parAction2) {
	if (0 == strcmp(parAction1, parAction2)) {
		return 1;
	}
	else {
		return 0;
	}
}
