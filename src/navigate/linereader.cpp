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

#include "linereader.hpp"
#include "listdircontent.hpp"
#include <cstdlib>
#include <cstring>
#include <memory>
#include <readline/readline.h>
#include <readline/history.h>
#include <cassert>
#include <ciso646>

namespace din {
	namespace {
		char* custom_generator (const char* parText, int parState) {
			static int list_index, len;

			if (not parState) {
				list_index = 0;
				len = std::strlen(parText);
			}
			return nullptr;
		}

		//char* custom_generator (const char* parText, int parState) {
		//}

		//char** custom_completion (const char* parText, int parStart, int parEnd) {
		//	char** matches = nullptr;

		//	if (0 == parStart) {
		//		matches = rl_completion_matches(const_cast<char*>(parText), &custom_generator);
		//	}
		//	else {
		//		//See the hack described here:
		//		//http://cc.byexamples.com/2008/06/16/gnu-readline-implement-custom-auto-complete/
		//		rl_bind_key('\t', &rl_abort);
		//	}
		//	return matches;
		//}
	} //unnamed namespace

	LineReader::LineReader (const ListDirContent* parLS) :
		m_ls(parLS)
	{
		assert(m_ls);

		//rl_attempted_completion_function = &custom_completion;
		rl_completion_entry_function = &custom_generator;
		rl_bind_key('\t', &rl_complete);
	}

	std::string LineReader::read (const std::string& parMessage) {
		typedef std::unique_ptr<char, void(*)(void*)> RawCharMemory;

		RawCharMemory line(readline(parMessage.c_str()), &std::free);

		if (line) {
			if (*line) {
				add_history(line.get());
			}
			return std::string(line.get());
		}
		else {
			return std::string();
		}
	}
} //namespace din
