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
		DirIterator::DirIterator (DirIterator&& parOther) :
			m_current(std::move(parOther.m_current)),
			m_end(std::move(parOther.m_end)),
			m_base_path(std::move(parOther.m_base_path))
		{
		}

		DirIterator::DirIterator (VecIterator parBegin, VecIterator parEnd, std::unique_ptr<PathName>&& parBasePath) :
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

		DirIterator::~DirIterator() noexcept {
		}

		void DirIterator::increment() {
			assert(PathName(m_current->abs_path).pop_right() == *m_base_path);
			do {
				++m_current;
			} while(
				m_current != m_end and
				m_current->level == m_base_path->atom_count() + 1 and
				*m_base_path != PathName(m_current->abs_path).pop_right()
			);
		}

		auto DirIterator::distance_to (const DirIterator& parOther) const -> difference_type {
			assert(false); //TODO: write implementation
			return std::distance(m_current, parOther.m_current);
		}

		bool DirIterator::equal (const DirIterator& parOther) const {
			return
				(m_end == parOther.m_end) and
				(
					(m_current == parOther.m_current) or
					(is_end() and parOther.is_end())
				)
			;
		}

		auto DirIterator::dereference() const -> reference {
			return const_cast<reference>(*m_current);
		}

		bool DirIterator::is_end() const {
			const bool is_this_end =
				not m_base_path or
				(m_current == m_end) or
				(m_current->level != m_base_path->atom_count() + 1) /*or
				(*m_base_path != PathName(m_current->abs_path).pop_right())
			*/;
			return is_this_end;
		}
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

	SetListingView::SetListingView (const const_iterator& parIter) :
		m_begin(parIter.m_current),
		m_end(parIter.m_end)
	{
	}

	SetListingView::SetListingView (const SetListing& parListing) :
		m_begin(parListing.m_list.begin()),
		m_end(parListing.m_list.end())
	{
	}

	auto SetListingView::begin() const -> const_iterator {
		return cbegin();
	}

	auto SetListingView::cbegin() const -> const_iterator {
		std::unique_ptr<PathName> base_path;
		if (m_begin != m_end) {
			base_path.reset(new PathName(m_begin->abs_path));
		}
		return const_iterator(m_begin, m_end, std::move(base_path));
	}

	auto SetListingView::end() const -> const_iterator {
		return cend();
	}

	auto SetListingView::cend() const -> const_iterator {
		return const_iterator(m_end, m_end, std::unique_ptr<PathName>());
	}
} //namespace mchlib
