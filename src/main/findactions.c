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

#include "findactions.h"
#include "dindexerConfig.h"
#include "helpers/lengthof.h"
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <iso646.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct ActionListStruct {
	char** list;
	size_t count;
	size_t used;
} ActionList;

static void increase_actionlist ( const char* parName, ActionList* parList );
static void push_action ( const char* parName, ActionList* parList );
static void foreach_dir ( void(*parAction)(const char*, ActionList*), ActionList* parList );
static int strcmp_wrapper ( const void* parA, const void* parB );

void find_actions (char*** parOut, size_t* parCount) {
	ActionList list;

	list.count = 0;
	foreach_dir(&increase_actionlist, &list);
	if (0 == list.count) {
		return;
	}

	*parOut = (char**)malloc(sizeof(char*) * list.count);
	list.list = *parOut;
	list.used = 0;
	foreach_dir(&push_action, &list);
	qsort((void*)list.list, list.count, sizeof(char*), &strcmp_wrapper);

	*parCount = list.count;
}

void free_actions (char** parActions, size_t parCount) {
	size_t z;

	for (z = 0; z < parCount; ++z) {
		free(parActions[z]);
	}
	free(parActions);
}

static void foreach_dir (void(*parAction)(const char*, ActionList*), ActionList* parList) {
	DIR* d;
	struct dirent* dir;
	size_t z;
	struct stat st;
	char* path_buff;
	size_t path_buff_length;
	size_t search_path_length;
	size_t path_buff_curr_length;

	d = opendir(ACTIONS_SEARCH_PATH);
	if (d) {
		path_buff_length = lengthof(ACTIONS_SEARCH_PATH) + 512;
		path_buff = (char*)malloc(path_buff_length);
		strncpy(path_buff, ACTIONS_SEARCH_PATH, lengthof(ACTIONS_SEARCH_PATH));
		search_path_length = lengthof(ACTIONS_SEARCH_PATH) - 1;
		if ('/' != path_buff[search_path_length - 1]) {
			path_buff[search_path_length] = '/';
			++search_path_length;
		}

		while ((dir = readdir(d)) != NULL) {
			/* Check if current item is . or .. */
			if (not strcmp(".", dir->d_name) or not strcmp("..", dir->d_name)) {
				continue;
			}

			/* Check if it's a directory */
			path_buff_curr_length = search_path_length;
			strncpy(path_buff + path_buff_curr_length, dir->d_name, path_buff_length - path_buff_curr_length);
			path_buff_curr_length += strlen(dir->d_name);
			lstat(path_buff, &st);
			if (S_ISDIR(st.st_mode)) {
				strncpy(path_buff + path_buff_curr_length, "/", path_buff_length - path_buff_curr_length);
				++path_buff_curr_length;
				strncpy(path_buff + path_buff_curr_length, ACTION_PREFIX, path_buff_length - path_buff_curr_length);
				path_buff_curr_length += lengthof(ACTION_PREFIX) - 1;
				strncpy(path_buff + path_buff_curr_length, dir->d_name, path_buff_length - path_buff_curr_length);

				if (0 == access(path_buff, 0)) {
					(*parAction)(path_buff + search_path_length, parList);
				}
			}
			else {
				if (0 == strncmp(dir->d_name, ACTION_PREFIX, lengthof(ACTION_PREFIX) - 1)) {
					(*parAction)(dir->d_name, parList);
				}
			}
		}

		free(path_buff);
		closedir(d);
	}
}

static void increase_actionlist (const char* parName, ActionList* parList) {
	++parList->count;
}

static void push_action (const char* parName, ActionList* parList) {
	size_t name_len;

	if (parList->used == parList->count) {
		return;
	}

	name_len = strlen(parName);
	parList->list[parList->used] = (char*)malloc(1 + name_len);
	strcpy(parList->list[parList->used], parName);
	++parList->used;
}

static int strcmp_wrapper (const void* parA, const void* parB) {
	const char* const arg1 = *(const char**)parA;
	const char* const arg2 = *(const char**)parB;

	return strcmp(get_actionname(arg1), get_actionname(arg2));
}

const char* get_actionname (const char* parAction) {
	const char* cmd_name_start;

	cmd_name_start = strchr(parAction, '/');
	if (not cmd_name_start) {
		cmd_name_start = parAction;
	}
	else {
		++cmd_name_start;
	}
	return cmd_name_start + lengthof(ACTION_PREFIX) - 1;
}
