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

#include "builtin_feats.h"
#include <stdio.h>
#include "dindexerConfig.h"
#include "timestamp.h"

void print_builtin_feats() {
	print_version();

	printf("CONFIG_FILE_PATH = \"%s\"\n", CONFIG_FILE_PATH);
	printf("ACTIONS_SEARCH_PATH = \"%s\"\n", ACTIONS_SEARCH_PATH);
	printf("ACTION_PREFIX = \"%s\"\n", ACTION_PREFIX);
#if defined(WITH_MEDIA_AUTODETECT)
	printf("WITH_MEDIA_AUTODETECT = yes\n");
#else
	printf("WITH_MEDIA_AUTODETECT = no\n");
#endif
#if defined(NDEBUG)
	printf("NDEBUG = yes (Release build)\n");
#else
	printf("NDEBUG = no (Debug build)\n");
#endif
	printf("Built on %s (CMake %s)\n", CMAKE_SYSTEM, CMAKE_VERSION);
	printf("C compiler = %s %s\n", CMAKE_C_COMPILER_ID, CMAKE_C_COMPILER_VERSION);
	printf("C++ compiler = %s %s\n", CMAKE_CXX_COMPILER_ID, CMAKE_CXX_COMPILER_VERSION);
	printf("Build flags:\n");
	printf("  %s %s\n", CMAKE_CXX_COMPILER, CMAKE_CXX_FLAGS);
	printf("  %s %s\n", CMAKE_C_COMPILER, CMAKE_C_FLAGS);
	printf("DB_OWNER_NAME = \"%s\"\n", DB_OWNER_NAME);
#if defined(DINDEXER_BUILD_DATE)
	printf("BUILD_DATE = %s\n", DINDEXER_BUILD_DATE);
#endif
}

void print_version() {
#if VERSION_BETA
	char beta_str[2] = "b";
#else
	char beta_str[1] = "";
#endif

	printf("%s v%d.%d.%d%s\nRev %s\n",
		PROGRAM_NAME,
		VERSION_MAJOR,
		VERSION_MINOR,
		VERSION_PATCH,
		beta_str,
		VERSION_GIT
	);
}
