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

#include "dindexer-machinery/set_listing.hpp"
#include "pathname.hpp"
#include <utility>
#include <ciso646>
#include <cassert>
#include <algorithm>


namespace mchlib {
	namespace {
		bool file_record_data_lt (const FileRecordData& parLeft, const FileRecordData& parRight) {
			const FileRecordData& l = parLeft;
			const FileRecordData& r = parRight;
			return
				(l.level < r.level)
				or (l.level == r.level and l.is_directory and not r.is_directory)
				or (l.level == r.level and l.is_directory == r.is_directory and l.abs_path < r.abs_path)

				//sort by directory - parent first, children later
				//(level == o.level and is_dir and not o.is_dir)
				//or (level == o.level and is_dir == o.is_dir and path < o.path)
				//or (level > o.level + 1)
				//or (level + 1 == o.level and is_dir and not o.is_dir and path < o.path)
				//or (level + 1 == o.level and is_dir and not o.is_dir and path == PathName(o.path).dirname())
				//or (level == o.level + 1 and not (o.is_dir and not is_dir and o.path == PathName(path).dirname()))
				;
		}
	} //unnamed namespace

	namespace implem {
		template <bool Const>
		DirIterator<Const>::DirIterator (DirIterator<Const>&& parOther) :
			m_current(std::move(parOther.m_current)),
			m_end(std::move(parOther.m_end)),
			m_base_path(std::move(parOther.m_base_path))
		{
		}

		template <bool Const>
		template <bool OtherConst>
		DirIterator<Const>::DirIterator (DirIterator<OtherConst>&& parOther, typename std::enable_if<std::is_convertible<typename DirIterator<OtherConst>::VecIterator, VecIterator>::value, enabler>::type) :
			m_current(parOther.m_current),
			m_end(parOther.m_end),
			m_base_path(std::move(parOther.m_base_path))
		{
		}

		template <bool Const>
		DirIterator<Const>::DirIterator (VecIterator parBegin, VecIterator parEnd, std::unique_ptr<PathName>&& parBasePath) :
			m_current(parBegin),
			m_end(parEnd),
			m_base_path(std::move(parBasePath))
		{
			assert(m_base_path or m_current == m_end);
			assert(m_current == m_end or m_base_path->atom_count() == m_current->level);

			//Look for the point where the children of this entry starts
			while (
				m_current != m_end and (
					m_current->level == m_base_path->atom_count() or
					*m_base_path != PathName(m_current->abs_path).pop_right()
			)) {
				assert(m_base_path);
				++m_current;
			}
		}

		template <bool Const>
		DirIterator<Const>::~DirIterator() noexcept {
		}

		template <bool Const>
		void DirIterator<Const>::increment() {
			assert(PathName(m_current->abs_path).pop_right() == *m_base_path);
			do {
				++m_current;
			} while(
				m_current != m_end and
				m_current->level == m_base_path->atom_count() + 1 and
				*m_base_path != PathName(m_current->abs_path).pop_right()
			);
		}

		template <bool Const>
		auto DirIterator<Const>::distance_to (const DirIterator<Const>& parOther) const -> difference_type {
			assert(false); //TODO: write implementation
			return std::distance(m_current, parOther.m_current);
		}

		template <bool Const>
		template <bool OtherConst>
		bool DirIterator<Const>::equal (const DirIterator<OtherConst>& parOther) const {
			return
				(m_end == parOther.m_end) and
				(
					(m_current == parOther.m_current) or
					(is_end() and parOther.is_end())
				)
			;
		}

		template <bool Const>
		auto DirIterator<Const>::dereference() const -> reference {
			return const_cast<reference>(*m_current);
		}

		template <bool Const>
		bool DirIterator<Const>::is_end() const {
			const bool is_this_end =
				not m_base_path or
				(m_current == m_end) or
				(m_current->level != m_base_path->atom_count() + 1) /*or
				(*m_base_path != PathName(m_current->abs_path).pop_right())
			*/;
			return is_this_end;
		}

		template class DirIterator<true>;
		template class DirIterator<false>;
		template bool DirIterator<true>::equal ( const DirIterator<false>& ) const;
		template bool DirIterator<true>::equal ( const DirIterator<true>& ) const;
		template bool DirIterator<false>::equal ( const DirIterator<false>& ) const;
		template bool DirIterator<false>::equal ( const DirIterator<true>& ) const;
		template DirIterator<true>::DirIterator ( DirIterator<false>&&, enabler );
	} //namespace implem

	SetListing::SetListing (ListType&& parList, bool parSort) :
		m_list(std::move(parList))
	{
		if (parSort) {
			std::sort(m_list.begin(), m_list.end(), &file_record_data_lt);
		}
	}

	SetListing::~SetListing() noexcept {
	}

	auto SetListing::begin() const -> const_iterator {
		return cbegin();
	}

	auto SetListing::cbegin() const -> const_iterator {
		std::unique_ptr<PathName> base_path;
		if (m_list.begin() != m_list.end()) {
			base_path.reset(new PathName(m_list.front().abs_path));
		}
		return const_iterator(m_list.begin(), m_list.end(), std::move(base_path));
	}

	auto SetListing::end() const -> const_iterator {
		return cend();
	}

	auto SetListing::cend() const -> const_iterator {
		return const_iterator(m_list.end(), m_list.end(), std::unique_ptr<PathName>());
	}

	SetListingView<false> SetListing::make_view() {
		return SetListingView<false>(m_list.begin(), m_list.end());
	}

	SetListingView<true> SetListing::make_view() const {
		return SetListingView<true>(m_list.begin(), m_list.end());
	}

	SetListingView<true> SetListing::make_cview() const {
		return SetListingView<true>(m_list.begin(), m_list.end());
	}

	template <bool Const>
	SetListingView<Const>::SetListingView (const implem::DirIterator<Const>& parIter) :
		m_begin(parIter.m_current),
		m_end(parIter.m_end)
	{
	}

	template <bool Const>
	SetListingView<Const>::SetListingView (list_iterator parBeg, list_iterator parEnd) :
		m_begin(std::move(parBeg)),
		m_end(std::move(parEnd))
	{
	}

	template <bool Const>
	auto SetListingView<Const>::begin() const -> const_iterator {
		return cbegin();
	}

	template <bool Const>
	auto SetListingView<Const>::cbegin() const -> const_iterator {
		std::unique_ptr<PathName> base_path;
		if (m_begin != m_end) {
			base_path.reset(new PathName(m_begin->abs_path));
		}
		return const_iterator(m_begin, m_end, std::move(base_path));
	}

	template <bool Const>
	auto SetListingView<Const>::end() const -> const_iterator {
		return cend();
	}

	template <bool Const>
	auto SetListingView<Const>::cend() const -> const_iterator {
		return const_iterator(m_end, m_end, std::unique_ptr<PathName>());
	}

	template <bool Const>
	template <bool B, typename R>
	R SetListingView<Const>::begin() {
		std::unique_ptr<PathName> base_path;
		if (m_begin != m_end) {
			base_path.reset(new PathName(m_begin->abs_path));
		}
		return iterator(m_begin, m_end, std::move(base_path));
	}

	template <bool Const>
	template <bool B, typename R>
	R SetListingView<Const>::end() {
		return iterator(m_end, m_end, std::unique_ptr<PathName>());
	}

	template class SetListingView<true>;
	template class SetListingView<false>;
	template SetListingView<false>::iterator SetListingView<false>::begin ( void );
	template SetListingView<false>::iterator SetListingView<false>::end ( void );
} //namespace mchlib
