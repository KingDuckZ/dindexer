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

#ifndef id279E04E31E2C4D98B8C902781A3CE018
#define id279E04E31E2C4D98B8C902781A3CE018

#include <vector>
#include <string>

namespace din {
	class PathName {
	public:
		explicit PathName ( const char* parPath );
		~PathName ( void ) noexcept = default;

		bool is_absolute ( void ) const;
		std::string path ( void ) const;

	private:
		std::vector<std::string> m_atoms;
		bool m_absolute;
	};
} //namespace din

#endif
