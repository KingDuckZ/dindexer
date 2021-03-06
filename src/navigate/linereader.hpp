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

#ifndef idBB92A7743E75400CBA486A241F13D35C
#define idBB92A7743E75400CBA486A241F13D35C

#include <string>

namespace din {
	class ListDirContent;

	class LineReader {
	public:
		explicit LineReader ( const ListDirContent* parLS );
		~LineReader ( void ) noexcept = default;

		std::string read ( const std::string& parMessage, const std::string& parCurrPath );

	private:
		const ListDirContent* m_ls;
	};
} //namespace din

#endif
