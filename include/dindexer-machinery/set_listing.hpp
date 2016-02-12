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
	class SetListingView;

	namespace implem {
		class DirIterator : public boost::iterator_facade<DirIterator, FileRecordData, boost::forward_traversal_tag> {
			friend class mchlib::SetListingView;
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
			difference_type distance_to ( const DirIterator& parOther ) const;
			bool equal ( const DirIterator& parOther ) const;
			reference dereference ( void ) const;
			bool is_end ( void ) const;

			VecIterator m_current;
			VecIterator m_end;
			std::unique_ptr<PathName> m_base_path;
		};
	};

	class SetListingView {
	public:
		typedef implem::DirIterator const_iterator;
		typedef std::vector<FileRecordData>::const_iterator list_iterator;

		explicit SetListingView ( const const_iterator& parIter );
		SetListingView ( list_iterator parBeg, list_iterator parEnd );
		SetListingView ( SetListingView&& ) = default;
		~SetListingView ( void ) noexcept = default;

		const_iterator begin ( void ) const;
		const_iterator cbegin ( void ) const;
		const_iterator end ( void ) const;
		const_iterator cend ( void ) const;

	private:
		list_iterator m_begin;
		list_iterator m_end;
	};

	class SetListing {
	public:
		typedef std::vector<FileRecordData> ListType;
		typedef SetListingView::const_iterator const_iterator;

		explicit SetListing ( ListType&& parList, bool parSort=true );
		~SetListing ( void ) noexcept;

		const_iterator begin ( void ) const;
		const_iterator cbegin ( void ) const;
		const_iterator end ( void ) const;
		const_iterator cend ( void ) const;

		//ListType descend_copy ( const const_iterator& parItem ) const;
		SetListingView make_view ( void ) const;

	private:
		ListType m_list;
	};
} //namespace mchlib

#endif
