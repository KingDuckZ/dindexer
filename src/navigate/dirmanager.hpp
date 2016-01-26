/* Copyright 2016, Michele Santullo
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

#ifndef id0F3472647B3E45AE8A30319AE7C76477
#define id0F3472647B3E45AE8A30319AE7C76477

#include <vector>
#include <string>

namespace din {
	class DirManager {
	public:
		DirManager ( void ) = default;

		void push_piece ( const std::string& parPiece );
		std::string to_string ( void ) const;

	private:
		using StackType = std::vector<std::string>;

		StackType m_stack;
	};
} //namespace din

#endif
