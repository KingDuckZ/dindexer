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

#ifndef id031383D06E3E4D6FBD77625350CDF083
#define id031383D06E3E4D6FBD77625350CDF083

#include <stdexcept>
#include <string>
#include <cstddef>

namespace pq {
	class DatabaseException : public std::runtime_error {
	public:
		DatabaseException ( void ) = delete;
		~DatabaseException ( void ) noexcept = default;
		DatabaseException ( std::string&& parMsg, std::string&& parErr, const std::string& parFile, std::size_t parLine );

		const std::string& error_message ( void ) const noexcept { return m_error; }

	private:
		const std::string m_error;
	};
} //namespace pq

#endif
