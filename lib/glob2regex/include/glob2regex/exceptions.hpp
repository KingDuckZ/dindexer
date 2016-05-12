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

#ifndef id998F0978CB064D728DDD7BE77F1FD9DC
#define id998F0978CB064D728DDD7BE77F1FD9DC

#include <stdexcept>
#include <string>

namespace g2r {
	class ParsingError : public std::runtime_error {
	public:
		ParsingError ( void ) = delete;
		~ParsingError ( void ) noexcept = default;
		ParsingError ( const std::string& parGlob, unsigned int parPosition );

		const std::string& glob ( void ) const noexcept;
		unsigned int position ( void ) const noexcept;

	private:
		std::string m_glob;
		unsigned int m_position;
	};
} //namespace g2r

#endif
