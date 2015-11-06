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

#ifndef id9CF5E6FA7E334DF09559C2968C494CB9
#define id9CF5E6FA7E334DF09559C2968C494CB9

#include <string>
#include <boost/utility/string_ref.hpp>
#include <vector>
#include <utility>
#include <ciso646>
#include <cstdint>
#include <algorithm>
#include <functional>
#include <boost/iterator/transform_iterator.hpp>

namespace din {
	template <typename C, typename Str=std::basic_string<C>, typename StrRef=boost::basic_string_ref<C>>
	class StringPool {
		typedef std::pair<StrRef, const Str*> StringListPair;
		typedef std::vector<std::pair<Str, std::size_t>> PoolType;
		typedef std::vector<StringListPair> StringListType;
		typedef std::function<StrRef(const StringListPair&)> FuncGetFirst;

	public:
		typedef C char_type;
		typedef Str string_type;
		typedef StrRef stringref_type;
		typedef boost::transform_iterator<FuncGetFirst, typename StringListType::const_iterator> const_iterator;

		StringPool ( void ) = default;
		~StringPool ( void ) noexcept = default;

		template <typename ItR>
		void update ( ItR parDataBeg, ItR parDataEnd );
		void update ( const StringPool& parOther );
		void insert ( const std::vector<stringref_type>& parStrings, const string_type* parBaseString );
		void insert ( stringref_type parString, const string_type* parBaseString );
		const string_type* ptr_to_literal ( const char* parLiteral );
		std::size_t size ( void ) const { return m_strings.size(); }
		bool empty ( void ) const { return m_strings.empty(); }
		const_iterator begin ( void ) const;
		const_iterator end ( void ) const;
		const string_type* get_stringref_source ( std::size_t parIndex ) const;

	private:
		PoolType m_pool;
		StringListType m_strings;
	};
} //namespace din

#include "stringpool.inl"

#endif
