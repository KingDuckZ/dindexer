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

#ifndef id040FEEC20F7B4F65A3EF67BA6460E737
#define id040FEEC20F7B4F65A3EF67BA6460E737

#include "dindexer-machinery/recorddata.hpp"
#include "kakoune/safe_ptr.hh"
#include <vector>
#include <boost/iterator/iterator_facade.hpp>
#include <memory>
#include <type_traits>
#include <ciso646>
#include <cstdint>

namespace mchlib {
	namespace implem {
		template <bool Const> class DirIterator;
	} //namespace implem

	class PathName;
	template <bool Const> class SetListingView;

	template <bool Const>
	implem::DirIterator<Const> first_file ( const SetListingView<Const>& parList );
	template <bool Const>
	implem::DirIterator<Const> first_file ( SetListingView<Const>& parList );

	namespace implem {
		template <bool Const>
		class DirIterator : public boost::iterator_facade<DirIterator<Const>, FileRecordData, boost::forward_traversal_tag> {
			friend class mchlib::SetListingView<Const>;
			friend class boost::iterator_core_access;
			template <bool> friend class DirIterator;
			typedef boost::iterator_facade<DirIterator<Const>, FileRecordData, boost::forward_traversal_tag> base_class;
			struct enabler {};
		public:
			typedef typename std::conditional<
				Const,
				std::vector<mchlib::FileRecordData>::const_iterator,
				std::vector<mchlib::FileRecordData>::iterator
			>::type VecIterator;
			typedef typename base_class::difference_type difference_type;
			typedef typename base_class::value_type value_type;
			typedef typename base_class::pointer pointer;
			typedef typename base_class::reference reference;
			typedef typename base_class::iterator_category iterator_category;

			DirIterator ( DirIterator&& parOther );
			DirIterator ( const DirIterator& parOther );
			template <bool OtherConst>
			DirIterator ( DirIterator<OtherConst>&& parOther, typename std::enable_if<std::is_convertible<typename DirIterator<OtherConst>::VecIterator, VecIterator>::value, enabler>::type = enabler() );
			DirIterator ( VecIterator parBegin, VecIterator parEnd, const PathName* parBasePath, std::size_t parLevelOffset );
			~DirIterator ( void ) noexcept;

			DirIterator& operator= ( DirIterator&& parOther );
			DirIterator& operator= ( const DirIterator& parOther );
			template <bool OtherConst>
			DirIterator& operator= ( typename std::enable_if<std::is_convertible<typename DirIterator<OtherConst>::VecIterator, VecIterator>::value, DirIterator<OtherConst>>::type&& parOther );
			template <bool OtherConst>
			DirIterator& operator= ( const typename std::enable_if<std::is_convertible<typename DirIterator<OtherConst>::VecIterator, VecIterator>::value, DirIterator<OtherConst>>::type& parOther );

		private:
			void increment ( void );
			difference_type distance_to ( const DirIterator<Const>& parOther ) const;
			template <bool OtherConst>
			bool equal ( const DirIterator<OtherConst>& parOther ) const;

			reference dereference ( void ) const;
			bool is_end ( void ) const;

			VecIterator m_current;
			VecIterator m_end;
			Kakoune::SafePtr<const PathName> m_base_path;
			std::size_t m_level_offset;
		};
	};

	template <bool Const>
	class SetListingView {
		struct enabler {};
	public:
		typedef implem::DirIterator<true> const_iterator;
		typedef implem::DirIterator<false> iterator;
		typedef typename implem::DirIterator<Const>::VecIterator list_iterator;

		explicit SetListingView ( const implem::DirIterator<Const>& parIter );
		SetListingView ( list_iterator parBeg, list_iterator parEnd, std::size_t parLevelOffset );
		SetListingView ( list_iterator parBeg, list_iterator parEnd, std::size_t parLevelOffset, const std::shared_ptr<PathName>& parBasePath );
		template <bool B=not Const, typename Other=typename std::enable_if<B, SetListingView<not B>>::type>
		SetListingView ( const Other& parOther );
		SetListingView ( SetListingView&& ) = default;
		~SetListingView ( void ) noexcept = default;

		const_iterator begin ( void ) const;
		const_iterator cbegin ( void ) const;
		const_iterator end ( void ) const;
		const_iterator cend ( void ) const;
		template <bool B=not Const, typename R=typename std::enable_if<B, iterator>::type>
		R begin ( void );
		template <bool B=not Const, typename R=typename std::enable_if<B, iterator>::type>
		R end ( void );

	private:
		list_iterator m_begin;
		list_iterator m_end;
		std::shared_ptr<PathName> m_base_path;
		std::size_t m_level_offset;
	};

	using MutableSetListingView = SetListingView<false>;
	using ConstSetListingView = SetListingView<true>;

	class SetListing {
	public:
		typedef std::vector<FileRecordData> ListType;
		typedef std::vector<ShortFileRecordData> ShortListType;
		typedef implem::DirIterator<true> const_iterator;

		explicit SetListing ( ListType&& parList, bool parSort=true );
		~SetListing ( void ) noexcept;

		const_iterator begin ( void ) const;
		const_iterator cbegin ( void ) const;
		const_iterator end ( void ) const;
		const_iterator cend ( void ) const;

		//ListType descend_copy ( const const_iterator& parItem ) const;
		SetListingView<false> make_view ( void );
		SetListingView<true> make_view ( void ) const;
		SetListingView<true> make_cview ( void ) const;

		bool empty ( void ) const;
		std::size_t size ( void ) const;
		std::size_t files_count ( void ) const;
		std::size_t dir_count ( void ) const;
		const ListType& sorted_list ( void ) const;

		static void sort_list ( ListType& parList );
		static ListType::iterator lower_bound ( ListType& parList, const char* parPath, uint16_t parLevel, bool parIsDir );
		static ShortListType::iterator lower_bound ( ShortListType& parList, const char* parPath, uint16_t parLevel, bool parIsDir );

	private:
		ListType m_list;
		std::shared_ptr<PathName> m_base_path;
	};

	template <bool Const>
	inline implem::DirIterator<Const> first_file (const SetListingView<Const>& parList) {
		auto end = parList.end();
		for (auto it = parList.begin(); it != end; ++it) {
			if (not it->is_directory)
				return std::move(it);
		}
		return parList.end();
	}

	template <bool Const>
	inline implem::DirIterator<Const> first_file (SetListingView<Const>& parList) {
		auto end = parList.end();
		for (auto it = parList.begin(); it != end; ++it) {
			if (not it->is_directory)
				return std::move(it);
		}
		return parList.end();
	}
} //namespace mchlib

#endif
