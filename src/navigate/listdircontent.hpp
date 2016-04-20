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

#ifndef id5FCFB397A56A4D0D99AB6947A08E1075
#define id5FCFB397A56A4D0D99AB6947A08E1075

#include <boost/circular_buffer.hpp>
#include <utility>
#include <string>

namespace din {
	class GenericPath;
	class DBSource;

	class ListDirContent {
		using ListType = std::vector<std::string>;
		using CachedItemType = std::pair<std::string, ListType>;
	public:
		explicit ListDirContent ( DBSource* parDB );
		~ListDirContent ( void ) noexcept = default;

		const ListType& ls ( const GenericPath& parDir ) const;

	private:
		mutable boost::circular_buffer<CachedItemType> m_cache;
		DBSource* m_db;
	};
} //namespace din

#endif