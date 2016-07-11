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

#ifndef id4FAEF395B9ED47CB9D6B50B54C9A289A
#define id4FAEF395B9ED47CB9D6B50B54C9A289A

#include <cstddef>

namespace dinhelp {
	namespace bt {
		template <typename T, T... I>
		struct number_seq {
		};

		template <std::size_t... I>
		using index_seq = number_seq<std::size_t, I...>;

		namespace implem {
			template <typename T, T MIN, T MAX, T... I>
			struct range_builder;

			template <typename T, T MIN, T... I>
			struct range_builder<T, MIN, MIN, I...> {
				typedef number_seq<T, I...> type;
			};

			template <typename T, T MIN, T N, T... I>
			struct range_builder : public range_builder<T, MIN, N - 1, N - 1, I...> {
			};
		} //namespace implem

		template <typename T, T MIN, T MAX>
		using number_range = typename implem::range_builder<T, MIN, MAX>::type;

		template <std::size_t MIN, std::size_t MAX>
		using index_range = number_range<std::size_t, MIN, MAX>;
	} //namespace bt
} //namespace dinhelp

#endif
