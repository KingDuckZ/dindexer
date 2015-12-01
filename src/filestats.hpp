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

#ifndef id4A7D7AB671954418939FC0BDA19C5B3F
#define id4A7D7AB671954418939FC0BDA19C5B3F

#include <ctime>

namespace fastf {
	struct FileStats {
		int level;
		std::time_t atime;
		std::time_t mtime;
		bool is_dir;
		bool is_symlink;
	};
} //namespace fastf

#endif
