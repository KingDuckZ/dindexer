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

#ifndef idAA901DA47E234E37B325B3192EF50423
#define idAA901DA47E234E37B325B3192EF50423

//Use this function to turn a 1D sequence of elements that are logically grouped
//together by position into a 2D list. For example, if you have an input sequence
//like {A1, B1, C1, A2, B2, C2, A3, B3, C3} and you want to turn it into
//{ {A1, B1, C1}, {A2, B2, C2}, {A3, B3, C3} } you can use these functions to
//achieve that. You need to specify the size of the innermost groups, 3 in the
//example just given, and push_back() will automatically move to the next
//sublist every 3 items.

#include <cstddef>
#include <cassert>
#include <ciso646>
#include <stdexcept>
#include <utility>

namespace dinhelp {
	template <typename OuterList, typename InnerList=typename OuterList::value_type, typename InnerVal=typename InnerList::value_type>
	class FlatInsertIn2DList {
	public:
		typedef OuterList list_type;
		typedef InnerVal value_type;
		typedef InnerList inner_list_type;

		FlatInsertIn2DList ( list_type* parList, std::size_t parInnerCount, std::size_t parOuterCount=0 );
		void push_back ( value_type&& parValue );
		void push_back ( const value_type& parValue );
		std::size_t size ( void ) const;

	private:
		void expand_array ( void );

		list_type* const m_list;
		const std::size_t m_inner_count;
		const std::size_t m_outer_count;
	};
} //namespace dinhelp

#include "implem/flatinsertin2dlist.inl"

#endif
