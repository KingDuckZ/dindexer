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

#ifndef id279E04E31E2C4D98B8C902781A3CE018
#define id279E04E31E2C4D98B8C902781A3CE018

#include "stringpool.hpp"
#include "kakoune/safe_ptr.hh"
#include <vector>
#include <string>
#include <boost/utility/string_ref.hpp>
#include <map>
#include <iostream>

namespace mchlib {
	class PathName : public Kakoune::SafeCountable {
	public:
		PathName ( PathName&& ) = default;
		PathName ( const PathName& ) = default;
		explicit PathName ( boost::string_ref parPath );
		~PathName ( void ) noexcept = default;

		bool is_absolute ( void ) const { return m_absolute; }
		std::string path ( void ) const;
		std::size_t str_path_size ( void ) const;
		const std::string& original_path ( void ) const { return (m_original_path ? *m_original_path : m_empty_str); }
		std::size_t atom_count ( void ) const;
		const boost::string_ref operator[] ( std::size_t parIndex ) const;
		void join ( const PathName& parOther );
		void join ( const char* parOther );
		void join ( boost::string_ref parOther, const std::string* parSource );
		const std::string* get_stringref_source ( std::size_t parIndex ) const;
		std::string dirname ( void ) const;
		PathName& pop_right ( void );
		bool operator!= ( const PathName& parOther ) const;
		bool operator== ( const PathName& parOther ) const;

	private:
		static const std::string m_empty_str;

		StringPool<char> m_pool;
		const std::string* m_original_path;
		bool m_absolute;
	};

	PathName make_relative_path ( const PathName& parBasePath, const PathName& parOtherPath );
	std::ostream& operator<< ( std::ostream& parStream, const PathName& parPath );
} //namespace mchlib

#endif
