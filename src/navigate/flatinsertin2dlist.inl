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

namespace din {
	template <typename OuterList, typename InnerList, typename InnerVal>
	FlatInsertIn2DList<OuterList, InnerList, InnerVal>::FlatInsertIn2DList (list_type* parList, std::size_t parInnerCount, std::size_t parOuterCount) :
		m_list(parList),
		m_inner_count(parInnerCount),
		m_outer_count(parOuterCount)
	{
		assert(m_list);
		if (m_outer_count) {
			m_list->reserve(m_outer_count);
		}
	}

	template <typename OuterList, typename InnerList, typename InnerVal>
	void FlatInsertIn2DList<OuterList, InnerList, InnerVal>::push_back (value_type&& parValue) {
		expand_array();
		const auto curr_outer_index = m_list->size() - 1;
		(*m_list)[curr_outer_index].push_back(std::move(parValue));
	}

	template <typename OuterList, typename InnerList, typename InnerVal>
	void FlatInsertIn2DList<OuterList, InnerList, InnerVal>::push_back (const value_type& parValue) {
		expand_array();
		const auto curr_outer_index = m_list->size() - 1;
		(*m_list)[curr_outer_index].push_back(parValue);
	}

	template <typename OuterList, typename InnerList, typename InnerVal>
	void FlatInsertIn2DList<OuterList, InnerList, InnerVal>::expand_array() {
		auto& list = *m_list;
		const auto inner_size = (list.empty() ? m_inner_count : list[list.size() - 1].size());
		if (inner_size == m_inner_count) {
			if (not m_outer_count or list.size() < m_outer_count) {
				list.push_back(inner_list_type());
				list[list.size() - 1].reserve(m_inner_count);
			}
			else {
				throw std::runtime_error("Outer list is full");
			}
		}

		assert(not m_outer_count or list.size() <= m_outer_count);
		assert(list.size() > 0);
		assert(list[list.size() - 1].size() <= m_inner_count);
	}

	template <typename OuterList, typename InnerList, typename InnerVal>
	std::size_t FlatInsertIn2DList<OuterList, InnerList, InnerVal>::size() const {
		if (m_list->empty()) {
			return 0;
		}
		else {
			return (m_list->size() - 1) * m_inner_count + m_list[m_list->size() - 1].size();
		}
	}
} //namespace din
