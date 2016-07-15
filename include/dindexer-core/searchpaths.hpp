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

#ifndef idDD90110B9C7E44C4AAF8B46A663B11DC
#define idDD90110B9C7E44C4AAF8B46A663B11DC

#include <vector>
#include <initializer_list>
#include <string>
#include <boost/utility/string_ref.hpp>
#include <functional>

namespace dincore {
	class SearchPaths {
	public:
		enum SearchType {
			Directory,
			File,
			Any
		};
		using Predicate = std::function<bool(boost::string_ref, const std::string& parPath)>;

		SearchPaths ( void ) = default;
		explicit SearchPaths ( std::vector<std::string>&& parList );
		SearchPaths ( std::initializer_list<std::string> parInit );
		~SearchPaths ( void ) noexcept;

		void add_path ( std::string&& parPath );
		std::string first_hit ( boost::string_ref parFile, SearchType parType=Any ) const;
		std::string first_hit ( Predicate parPredicate, SearchType parType=Any ) const;

	private:
		std::vector<std::string> m_paths;
	};

	class ShallowSearchPaths {
	public:
		ShallowSearchPaths ( void ) = default;
		explicit ShallowSearchPaths ( std::vector<boost::string_ref>&& parList );
		ShallowSearchPaths ( std::initializer_list<boost::string_ref> parInit );
		~ShallowSearchPaths ( void ) noexcept;

		void add_path ( boost::string_ref parPath );
		std::string first_hit ( boost::string_ref parFile, SearchPaths::SearchType parType=SearchPaths::Any ) const;
		std::string first_hit ( SearchPaths::Predicate parPredicate, SearchPaths::SearchType parType=SearchPaths::Any ) const;

	private:
		std::vector<boost::string_ref> m_paths;
	};
} //namespace dincore

#endif
