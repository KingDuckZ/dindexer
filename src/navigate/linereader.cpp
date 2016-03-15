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
#include <cstdlib>
#include <memory>
#include <readline/readline.h>
#include <readline/history.h>
#include <cassert>

namespace din {
	LineReader::LineReader() {
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
