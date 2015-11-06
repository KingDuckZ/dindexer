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

#include "stringpool.hpp"
#include <vector>
#include <string>
#include <boost/utility/string_ref.hpp>
#include <map>

namespace din {
	class PathName {
	public:
		PathName ( PathName&& ) = default;
		PathName ( const PathName& ) = default;
		explicit PathName ( boost::string_ref parPath );
		~PathName ( void ) noexcept = default;

		bool is_absolute ( void ) const { return m_absolute; }
		std::string path ( void ) const;
		const std::string& original_path ( void ) const { return (m_original_path ? *m_original_path : m_empty_str); }
		std::size_t atom_count ( void ) const;
		const boost::string_ref operator[] ( std::size_t parIndex ) const;
		void join ( const PathName& parOther );
		void join ( const char* parOther );
		void join ( boost::string_ref parOther, const std::string* parSource );
		const std::string* get_stringref_source ( std::size_t parIndex ) const;

	private:
		static const std::string m_empty_str;

		StringPool<char> m_pool;
		const std::string* m_original_path;
		bool m_absolute;
	};

	PathName make_relative_path ( const PathName& parBasePath, const PathName& parOtherPath );
} //namespace din

#endif
