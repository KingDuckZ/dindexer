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
#include <type_traits>

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
	std::vector<typename std::conditional<Const, const FileRecordData*, FileRecordData*>::type>
	flattened_listing ( const mchlib::SetListingView<Const>& parContent );

	namespace implem {
		template <bool Const>
		void flattened_listing (const mchlib::SetListingView<Const>& parContent, std::vector<typename std::conditional<Const, const FileRecordData*, FileRecordData*>::type>& parOut) {
			const auto end = parContent.end();

			for (auto itcurr = parContent.cbegin(); itcurr != end; ++itcurr) {
				parOut.push_back(&*itcurr);

				if (itcurr->is_directory) {
					flattened_listing(mchlib::SetListingView<Const>(itcurr), parOut);
				}
			}
		}
	} //namespace implem

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
	std::size_t count_listing_items (const SetListingView<Const>& parList) {
		return std::count_if(
			parList.cbegin(),
			parList.cend(),
			[] (const FileRecordData&) {
				return true;
			}
		);
	}

	template <bool Const>
	inline
	std::size_t count_listing_items_recursive (const SetListingView<Const>& parList) {
		std::size_t retval = 0;
		for (auto it = parList.begin(), itEND = parList.end(); it != itEND; ++it, ++retval) {
			if (it->is_directory)
				retval += count_listing_items_recursive(SetListingView<Const>(it));
		}
		return retval;
	}

	template <bool Const>
	inline
	std::vector<typename std::conditional<Const, const FileRecordData*, FileRecordData*>::type>
	flattened_listing (const mchlib::SetListingView<Const>& parContent) {
		std::vector<typename std::conditional<Const, const FileRecordData*, FileRecordData*>::type> retval;

		implem::flattened_listing(parContent, retval);
		return retval;
	}
} //namespace mchlib

#endif
