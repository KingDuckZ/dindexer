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
#include <boost/utility/string_ref.hpp>

namespace mchlib {
	namespace {
		//A struct that wraps the minimum necessary in order for LT comparison
		//to be made.
		struct FileRecordDataForSearch {
			FileRecordDataForSearch ( const char* parPath, uint16_t parLevel, bool parIsDir) :
				abs_path(parPath),
				level(parLevel),
				is_directory(parIsDir)
			{
				assert(parPath);
			}

			boost::string_ref abs_path;
			uint16_t level;
			bool is_directory;
		};

		template <typename OtherRecord>
		bool file_record_data_lt (const FileRecordData& parLeft, const OtherRecord& parRight) {
			const FileRecordData& l = parLeft;
			const OtherRecord& r = parRight;
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
		DirIterator<Const>::DirIterator (DirIterator&& parOther) :
			m_current(std::move(parOther.m_current)),
			m_end(std::move(parOther.m_end)),
			m_base_path(std::move(parOther.m_base_path)),
			m_level_offset(parOther.m_level_offset)
		{
		}

		template <bool Const>
		DirIterator<Const>::DirIterator (const DirIterator& parOther) :
			m_current(parOther.m_current),
			m_end(parOther.m_end),
			m_base_path(parOther.m_base_path),
			m_level_offset(parOther.m_level_offset)
		{
		}

		template <bool Const>
		template <bool OtherConst>
		DirIterator<Const>::DirIterator (DirIterator<OtherConst>&& parOther, typename std::enable_if<std::is_convertible<typename DirIterator<OtherConst>::VecIterator, VecIterator>::value, enabler>::type) :
			m_current(parOther.m_current),
			m_end(parOther.m_end),
			m_base_path(std::move(parOther.m_base_path)),
			m_level_offset(parOther.m_level_offset)
		{
		}

		template <bool Const>
		DirIterator<Const>::DirIterator (VecIterator parBegin, VecIterator parEnd, const PathName* parBasePath, std::size_t parLevelOffset) :
			m_current(parBegin),
			m_end(parEnd),
			m_base_path(parBasePath),
			m_level_offset(parLevelOffset)
		{
			assert(parBasePath);
			assert(m_base_path or m_current == m_end);
			assert(m_current == m_end or m_base_path->atom_count() == PathName(m_current->abs_path).atom_count());
			assert(m_current == m_end or m_base_path->atom_count() == m_current->level + m_level_offset);

			//Look for the point where the children of this entry start
			while (
				m_current != m_end and (
					m_current->level + m_level_offset == m_base_path->atom_count() or
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
		DirIterator<Const>& DirIterator<Const>::operator= (DirIterator&& parOther) {
			m_current = std::move(parOther.m_current);
			m_end = std::move(parOther.m_end);
			m_base_path = std::move(parOther.m_base_path);
			m_level_offset = parOther.m_level_offset;
			return *this;
		}

		template <bool Const>
		DirIterator<Const>& DirIterator<Const>::operator= (const DirIterator& parOther) {
			m_current = parOther.m_current;
			m_end = parOther.m_end;
			m_base_path = parOther.m_base_path;
			m_level_offset = parOther.m_level_offset;
			return *this;
		}

		template <bool Const>
		template <bool OtherConst>
		DirIterator<Const>& DirIterator<Const>::operator= (typename std::enable_if<std::is_convertible<typename DirIterator<OtherConst>::VecIterator, VecIterator>::value, DirIterator<OtherConst>>::type&& parOther) {
			m_current = parOther.m_current;
			m_end = parOther.m_end;
			m_base_path = std::move(parOther.m_base_path);
			m_level_offset = parOther.m_level_offset;
			return *this;
		}

		template <bool Const>
		template <bool OtherConst>
		DirIterator<Const>& DirIterator<Const>::operator= (const typename std::enable_if<std::is_convertible<typename DirIterator<OtherConst>::VecIterator, VecIterator>::value, DirIterator<OtherConst>>::type& parOther) {
			m_current = parOther.m_current;
			m_end = parOther.m_end;
			m_base_path = parOther.m_base_path;
			m_level_offset = parOther.m_level_offset;
			return *this;
		}

		template <bool Const>
		void DirIterator<Const>::increment() {
			assert(PathName(m_current->abs_path).pop_right() == *m_base_path);
			do {
				++m_current;
			} while(
				m_current != m_end and
				m_current->level + m_level_offset == m_base_path->atom_count() + 1 and
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
				(m_current->level + m_level_offset != m_base_path->atom_count() + 1) /*or
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
		m_list(std::move(parList)),
		m_base_path()
	{
		if (parSort) {
			sort_list(m_list);
		}
		else {
			//Assert that received list is already sorted
			assert(std::equal(m_list.begin(), m_list.end(), SetListing(ListType(m_list), true).sorted_list().begin()));
		}
		if (not m_list.empty()) {
			m_base_path.reset(new PathName(m_list.front().abs_path));
		}
	}

	SetListing::~SetListing() noexcept {
	}

	auto SetListing::begin() const -> const_iterator {
		return cbegin();
	}

	auto SetListing::cbegin() const -> const_iterator {
		const auto atom_count = (m_base_path ? m_base_path->atom_count() : 0);
		return const_iterator(m_list.begin(), m_list.end(), m_base_path.get(), atom_count);
	}

	auto SetListing::end() const -> const_iterator {
		return cend();
	}

	auto SetListing::cend() const -> const_iterator {
		return const_iterator(m_list.end(), m_list.end(), m_base_path.get(), 0);
	}

	bool SetListing::empty() const {
		return m_list.empty();
	}

	std::size_t SetListing::size() const {
		return m_list.size();
	}

	std::size_t SetListing::files_count() const {
		return std::count_if(
			m_list.begin(),
			m_list.end(),
			[] (const FileRecordData& parItm) {
				return not parItm.is_directory;
			}
		);
	}

	std::size_t SetListing::dir_count() const {
		return std::count_if(
			m_list.begin(),
			m_list.end(),
			[] (const FileRecordData& parItm) {
				return parItm.is_directory;
			}
		);
	}

	const SetListing::ListType& SetListing::sorted_list() const {
		return m_list;
	}

	void SetListing::sort_list (ListType& parList) {
		std::sort(parList.begin(), parList.end(), &file_record_data_lt<FileRecordData>);
	}

	SetListing::ListType::iterator SetListing::lower_bound (ListType& parList, const char* parPath, uint16_t parLevel, bool parIsDir) {
		using boost::string_ref;
		FileRecordDataForSearch find_record(parPath, parLevel, parIsDir);
		return std::lower_bound(parList.begin(), parList.end(), find_record, &file_record_data_lt<FileRecordDataForSearch>);
	}

	SetListingView<false> SetListing::make_view() {
		const auto offs = (m_list.empty() ? 0 : PathName(m_list.front().abs_path).atom_count());
		return SetListingView<false>(m_list.begin(), m_list.end(), offs, m_base_path);
	}

	SetListingView<true> SetListing::make_view() const {
		const auto offs = (m_list.empty() ? 0 : PathName(m_list.front().abs_path).atom_count());
		return SetListingView<true>(m_list.begin(), m_list.end(), offs, m_base_path);
	}

	SetListingView<true> SetListing::make_cview() const {
		const auto offs = (m_list.empty() ? 0 : PathName(m_list.front().abs_path).atom_count());
		return SetListingView<true>(m_list.begin(), m_list.end(), offs, m_base_path);
	}

	template <bool Const>
	SetListingView<Const>::SetListingView (const implem::DirIterator<Const>& parIter) :
		m_begin(parIter.m_current),
		m_end(parIter.m_end),
		m_base_path(),
		m_level_offset(parIter.m_level_offset)
	{
		if (m_begin != m_end) {
			m_base_path.reset(new PathName(m_begin->abs_path));
		}
	}

	template <bool Const>
	SetListingView<Const>::SetListingView (list_iterator parBeg, list_iterator parEnd, std::size_t parLevelOffset) :
		m_begin(std::move(parBeg)),
		m_end(std::move(parEnd)),
		m_base_path(),
		m_level_offset(parLevelOffset)
	{
		if (m_begin != m_end) {
			m_base_path.reset(new PathName(m_begin->abs_path));
		}
	}

	template <bool Const>
	SetListingView<Const>::SetListingView (list_iterator parBeg, list_iterator parEnd, std::size_t parLevelOffset, const std::shared_ptr<PathName>& parBasePath) :
		m_begin(std::move(parBeg)),
		m_end(std::move(parEnd)),
		m_base_path(parBasePath),
		m_level_offset(parLevelOffset)
	{
	}

	template <bool Const>
	template <bool B, typename Other>
	SetListingView<Const>::SetListingView (const Other& parOther) :
		m_begin(parOther.m_begin),
		m_end(parOther.m_end),
		m_base_path(parOther.m_base_path),
		m_level_offset(parOther.m_level_offset)
	{
	}

	template <bool Const>
	auto SetListingView<Const>::begin() const -> const_iterator {
		return cbegin();
	}

	template <bool Const>
	auto SetListingView<Const>::cbegin() const -> const_iterator {
		return const_iterator(m_begin, m_end, m_base_path.get(), m_level_offset);
	}

	template <bool Const>
	auto SetListingView<Const>::end() const -> const_iterator {
		return cend();
	}

	template <bool Const>
	auto SetListingView<Const>::cend() const -> const_iterator {
		return const_iterator(m_end, m_end, m_base_path.get(), m_level_offset);
	}

	template <bool Const>
	template <bool B, typename R>
	R SetListingView<Const>::begin() {
		return iterator(m_begin, m_end, m_base_path.get(), m_level_offset);
	}

	template <bool Const>
	template <bool B, typename R>
	R SetListingView<Const>::end() {
		return iterator(m_end, m_end, m_base_path.get(), m_level_offset);
	}

	template class SetListingView<true>;
	template class SetListingView<false>;
	template SetListingView<false>::iterator SetListingView<false>::begin ( void );
	template SetListingView<false>::iterator SetListingView<false>::end ( void );
} //namespace mchlib
