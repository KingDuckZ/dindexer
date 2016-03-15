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

#define _GNU_SOURCE 1

#include "dindexerConfig.h"
#include "findactions.h"
#include "helpers/lengthof.h"
#include "builtin_feats.h"
#include <string.h>
#include <stdio.h>
#include <iso646.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

struct PrintContext {
	FILE* stream;
	char* prefix_filter;
	int add_space;
};
typedef struct PrintContext PrintContext;

static size_t foreach_avail_action ( int(*parFunc)(const char*, const void*), char** parList, size_t parCount, const void* parPass );
static int printf_stream ( const char* parMsg, const void* parStream );
static int printf_stream_inplace ( const char* parMsg, const void* parPrintContext );
static int same_action ( const char* parAction1, const void* parAction2 );
static void print_usage ( void );
static int manage_commandline ( int parArgc, char* parArgv[], char** parActions, size_t parActionCount, int* parShouldQuit );

int main (int parArgc, char* parArgv[]) {
	size_t z;
	size_t actions_count;
	char** actions;
	char* action_path;
	char* specified_action;
	size_t action_path_length;
	size_t selected_action;
	char** argv;
	int retval;
	int should_quit;

	find_actions(&actions, &actions_count);
	retval = manage_commandline(parArgc, parArgv, actions, actions_count, &should_quit);
	if (should_quit) {
		free_actions(actions, actions_count);
		return retval;
	}
	if (0 == actions_count) {
		fprintf(stderr, "No actions found in \"%s\"\n", ACTIONS_SEARCH_PATH);
		return 1;
	}


	if (optind < parArgc)
		specified_action = parArgv[optind];
	else
		specified_action = "";
	selected_action = foreach_avail_action(&same_action, actions, actions_count, specified_action);

	if (actions_count == selected_action) {
		fprintf(stderr, "Unrecognized action \"%s\" - available actions are:\n", specified_action);
		foreach_avail_action(&printf_stream, actions, actions_count, stderr);
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
		argv[z - 1] = specified_action;
	}

	/*printf("would call %s\n", action_path);*/
	execv(action_path, argv);

	/* the program won't get here, but just to be clean... */
	free(action_path);
	return 0;
}

static size_t foreach_avail_action(int(*parFunc)(const char*, const void*), char** parList, size_t parCount, const void* parPass) {
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

static int printf_stream (const char* parMsg, const void* parStream) {
	FILE* stream = (FILE*)parStream;
	fprintf(stream, "\t%s\n", parMsg);
	return 0;
}

static int printf_stream_inplace (const char* parMsg, const void* parPrintContext) {
	size_t prefix_len;
	struct PrintContext* context = (PrintContext*)parPrintContext;
	const char* separator = (context->add_space ? "\n" : "");

	prefix_len = (context->prefix_filter ? strlen(context->prefix_filter) : 0);
	if (0 == prefix_len or strncmp(context->prefix_filter, parMsg, prefix_len) == 0) {
		context->add_space = 1;
		fprintf(context->stream, "%s%s", separator, parMsg);
	}
	return 0;
}

static int same_action (const char* parAction1, const void* parAction2) {
	const char* const action2 = (const char*)parAction2;
	if (0 == strcmp(parAction1, action2)) {
		return 1;
	}
	else {
		return 0;
	}
}

static void print_usage() {
	printf("--help, -h - show this help\n");
	printf("--builtin, -b - show build info\n");
	printf("--printactions=[prefix] - print a complete-friendly list of available commands, filtered by an optional prefix\n");
}

static int manage_commandline (int parArgc, char* parArgv[], char** parActions, size_t parActionCount, int* parShouldQuit) {
	int showbuiltin;
	int showhelp;
	int showactions_for_completion;
	int option_index;
	int getopt_retval;
	FILE* streamout;
	int retval;
	struct PrintContext actions_print_context;

	struct option opts[] = {
		{ "printactions", optional_argument, NULL, 'a' },
		{ "builtin", no_argument, &showbuiltin, 1 },
		{ "help", no_argument, &showhelp, 1 },
		{ 0, 0, 0, 0 }
	};

	memset(&actions_print_context, 0, sizeof(actions_print_context));
	option_index = 0;
	showbuiltin = showhelp = showactions_for_completion = 0;
	*parShouldQuit = 0;

	while (0 <= (getopt_retval = getopt_long(parArgc, parArgv, "bh", opts, &option_index))) {
		switch (getopt_retval) {
		case 'h':
			showhelp = 1;
			break;
		case 'b':
			showbuiltin = 1;
			break;
		case '?':
			*parShouldQuit = 1;
			return 2;
		case 'a':
			showactions_for_completion = 1;
			actions_print_context.prefix_filter = (optarg ? optarg : "");
		}
		option_index = 0;
	}

	if (parArgc < 2 or showhelp) {
		*parShouldQuit = 1;
		if (parArgc < 2) {
			streamout = stderr;
			fprintf(stderr, "No action specified. ");
			retval = 2;
		}
		else {
			print_usage();
			printf("\n");
			streamout = stdout;
			retval = 0;
		}
		fprintf(streamout, "Available actions are:\n");
		foreach_avail_action(&printf_stream, parActions, parActionCount, streamout);
		return retval;
	}
	else if (showbuiltin) {
		*parShouldQuit = 1;
		print_builtin_feats();
		return 0;
	}
	else if (showactions_for_completion) {
		*parShouldQuit = 1;
		actions_print_context.stream = stdout;
		foreach_avail_action(&printf_stream_inplace, parActions, parActionCount, &actions_print_context);
		if (actions_print_context.add_space) {
			printf("\n");
		}
	}
	return 0;
}
