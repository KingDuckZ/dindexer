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

#include "dindexer-machinery/recorddata.hpp"
#include <vector>
#include <boost/iterator/iterator_facade.hpp>
#include <memory>

#ifndef id040FEEC20F7B4F65A3EF67BA6460E737
#define id040FEEC20F7B4F65A3EF67BA6460E737

namespace mchlib {
	class PathName;

	namespace implem {
		class DirIterator : public boost::iterator_facade<DirIterator, FileRecordData, boost::random_access_traversal_tag> {
			friend class boost::iterator_core_access;
			typedef boost::iterator_facade<DirIterator, FileRecordData, boost::random_access_traversal_tag> base_class;
			typedef base_class::difference_type difference_type;
			typedef base_class::reference reference;
		public:
			typedef std::vector<mchlib::FileRecordData>::const_iterator VecIterator;

			DirIterator ( DirIterator&& parOther );
			DirIterator ( VecIterator parBegin, VecIterator parEnd, std::unique_ptr<PathName>&& parBasePath );
			~DirIterator ( void ) noexcept;

		private:
			void increment ( void );
			void decrement ( void );
			void advance ( std::size_t parAdvance );
			difference_type distance_to ( const DirIterator& parOther ) const;
			bool equal ( const DirIterator& parOther ) const;
			reference dereference ( void ) const;
			bool is_end ( void ) const;

			//There are three iterators and an offset because the first item
			//could very well be detached from the rest of its siblings. For
			//example "a", "b", "a/c", "a/d": when iterating on that sequence the
			//expected outcome is "a", "a/c", "a/d". So in order to be able to
			//iterate back and forward we need a "first" iterator so we can
			//always get back to it whene we are decrementing an iterator to
			//"a/c" for example. current is just the current iterator, and the
			//offset is needed so we can tell if at any moment we are at
			//first+offset and skip back to first instead of decrementing
			//current. end is just there so we know when to stop advancing.
			VecIterator m_first;
			VecIterator m_current;
			VecIterator m_end;
			std::unique_ptr<PathName> m_base_path;
			std::vector<FileRecordData>::difference_type m_second_offs;
		};
	};

	//class SetListingView {
	//public:
	//	typedef DirIterator const_iterator;

	//	SetListingView ( SetListing::const_iterator parBeg, SetListing::const_iterator parVeryEnd );
	//	~SetListingView ( void ) noexcept = default;

	//	const_iterator begin ( void ) const;
	//	const_iterator cbegin ( void ) const;
	//	const_iterator end ( void ) const;
	//	const_iterator cend ( void ) const;

	//	SetListingView

	//private:
	//	const SetListing::const_iterator m_begin;
	//	const SetListing::const_iterator m_end;
	//};

	class SetListing {
		typedef std::vector<FileRecordData> ListType;
	public:
		typedef implem::DirIterator const_iterator;

		explicit SetListing ( ListType&& parList, bool parSort=true );
		~SetListing ( void ) noexcept;

		const_iterator begin ( void ) const;
		const_iterator cbegin ( void ) const;
		const_iterator end ( void ) const;
		const_iterator cend ( void ) const;

		ListType descend_copy ( const const_iterator& parItem ) const;

	private:
		ListType m_list;
	};
} //namespace mchlib

#endif
