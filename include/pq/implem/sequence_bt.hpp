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

#ifndef id4FAEF395B9ED47CB9D6B50B54C9A289A
#define id4FAEF395B9ED47CB9D6B50B54C9A289A

#include <cstddef>

namespace pq {
	namespace bt {
		template <std::size_t... I>
		struct index_seq {
		};

		namespace implem {
			template <std::size_t MIN, std::size_t MAX, std::size_t... I>
			struct range_builder;

			template <std::size_t MIN, std::size_t... I>
			struct range_builder<MIN, MIN, I...> {
				typedef index_seq<I...> type;
			};

			template <std::size_t MIN, std::size_t N, std::size_t... I>
			struct range_builder : public range_builder<MIN, N - 1, N - 1, I...> {
			};
		} //namespace implem

		template <std::size_t MIN, std::size_t MAX>
		using index_range = typename implem::range_builder<MIN, MAX>::type;
	} //namespace bt
} //namespace pq

#endif
