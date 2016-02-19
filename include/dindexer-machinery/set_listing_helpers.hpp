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

#ifndef id036D985665EC437096DF7EBC5607DB0A
#define id036D985665EC437096DF7EBC5607DB0A

#include "set_listing.hpp"
#include <cstddef>
#include <algorithm>

namespace mchlib {
	template <bool Const>
	std::size_t count_listing_dirs ( const SetListingView<Const>& parList );

	template <bool Const>
	std::size_t count_listing_files ( const SetListingView<Const>& parList );

	template <bool Const>
	std::size_t count_listing_items ( const SetListingView<Const>& parList );

	template <bool Const>
	std::size_t count_listing_items_recursive ( const SetListingView<Const>& parList );

	template <bool Const>
	inline
	std::size_t count_listing_dirs (const SetListingView<Const>& parList) {
		return std::count_if(
			parList.cbegin(),
			parList.cend(),
			[] (const FileRecordData& parItm) {
				return parItm.is_directory;
			}
		);
	}

	template <bool Const>
	inline
	std::size_t count_listing_files (const SetListingView<Const>& parList) {
		return std::count_if(
			parList.cbegin(),
			parList.cend(),
			[] (const FileRecordData& parItm) {
				return not parItm.is_directory;
			}
		);
	}

	template <bool Const>
	inline
	std::size_t count_listing_items (const SetListingView<Const>& /*parList*/) {
		assert(false);
		return 0;
		//return std::count_if(
		//	parList.cbegin(),
		//	parList.cend(),
		//	[] (const FileRecordData&) {
		//		return true;
		//	}
		//);
	}
} //namespace mchlib

#endif
