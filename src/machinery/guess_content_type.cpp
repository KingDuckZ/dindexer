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

#ifndef idF5514EBEE7DA404CA2271A4AE74A28D4
#define idF5514EBEE7DA404CA2271A4AE74A28D4

#include "dindexer-machinery/guess_content_type.hpp"
#include "dindexer-machinery/set_listing.hpp"
#include "dindexer-machinery/set_listing_helpers.hpp"
#include "globbing.hpp"
#include "pathname.hpp"
#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/range/empty.hpp>
#include <cstdint>
#include <algorithm>
#include <functional>
#include <ciso646>
#include <regex>
#include <utility>
#include <memory>

namespace mchlib {
	namespace {
		template <typename O, uint16_t L>
		struct IsLevelLikeO {
			bool operator() ( const FileRecordData& parEntry );
		};

		struct EntryChecking {
			typedef bool(*CheckerFunction)(MediaTypes, const ConstSetListingView&, const std::vector<const FileRecordData*>&);

			std::size_t max_total_entries;
			CheckerFunction checker_func;
			ContentTypes content_type;
		};

		template <typename O, uint16_t L>
		bool IsLevelLikeO<O, L>::operator() (const FileRecordData& parEntry) {
			return O()(parEntry.level, L);
		}

		template <typename O, uint16_t L>
		std::vector<const FileRecordData*> fetch_entries (const std::vector<FileRecordData>& parContent) {
			using IsLevOk = IsLevelLikeO<O, L>;

			auto it_begin = boost::make_filter_iterator<IsLevOk>(parContent.begin(), parContent.end());
			auto it_end = boost::make_filter_iterator<IsLevOk>(parContent.end(), parContent.end());
			std::vector<const FileRecordData*> retval;
			for (; it_begin != it_end; ++it_begin) {
				retval.push_back(&*it_begin);
			}
			return std::move(retval);
		}

		std::vector<int> check_missing_content (const std::vector<const FileRecordData*>& parContent, const std::vector<const char*>& parCheck) {
			using boost::make_indirect_iterator;
			using FileRecordIterator =
				boost::indirect_iterator<std::vector<const FileRecordData*>::const_iterator>;

			std::vector<int> retval;
			auto glob = Glob<FileRecordIterator>(
				make_indirect_iterator(parContent.begin()),
				make_indirect_iterator(parContent.end())
			);

			for (int z = 0; z < static_cast<int>(parCheck.size()); ++z) {
				glob.set_pattern(parCheck[z]);
				if (boost::empty(glob)) {
					retval.push_back(z);
				}
			}
			return std::move(retval);
		}

		bool identify_video_dvd (MediaTypes parMediaType, const ConstSetListingView& parContent, const std::vector<const FileRecordData*>& parFlatContent ) {
			if (parMediaType != MediaType_DVD and parMediaType != MediaType_Directory)
				return false;

			const auto items_count = count_listing_items(parContent);
			if (items_count < 2) {
				return false;
			}

			const std::vector<const char*> should_have {
				"VIDEO_TS/*.VOB",
				"AUDIO_TS"
			};
			return check_missing_content(parFlatContent, should_have).empty();
		}

		bool identify_video_cd (MediaTypes parMediaType, const ConstSetListingView& parContent, const std::vector<const FileRecordData*>& parFlatContent) {
			if (parMediaType != MediaType_CDRom and parMediaType != MediaType_Directory)
				return false;

			const auto items_count = count_listing_items(parContent);
			if (items_count < 4)
				return false;

			const std::vector<const char*> should_have {
				"SVCD/*.VCD",
				"MPEGAV/AVSEQ??.DAT",
				"SEGMENT/ITEM???.DAT",
				"CDI"
			};
			return check_missing_content(parFlatContent, should_have).empty();
		}
	} //unnamed namespace

	ContentTypes guess_content_type (MediaTypes parMediaType, const ConstSetListingView& parContent, std::size_t parEntriesCount) {
		if (boost::empty(parContent))
			return ContentType_Empty;

		std::vector<EntryChecking> checker_chain {
			{ 100, &identify_video_dvd, ContentType_VideoDVD },
			{ 200, &identify_video_cd, ContentType_VideoCD }
		};

		const auto total_entries = (parEntriesCount ? parEntriesCount : count_listing_items_recursive(parContent));
		assert(total_entries == count_listing_items_recursive(parContent));
		auto flattened = flattened_listing(parContent);
		assert(flattened.size() == total_entries);

		for (const auto& chk : checker_chain) {
			if (chk.max_total_entries and chk.max_total_entries >= total_entries) {
				if (chk.checker_func(parMediaType, parContent, flattened)) {
					return chk.content_type;
				}
			}
		}
		return ContentType_Generic;
	}

	ContentTypes guess_content_type (MediaTypes parMediaType, const std::vector<FileRecordData>& parContent) {
		if (parContent.empty())
			return ContentType_Empty;

		//Assert that array being passed in is sorted in the same way SetListing
		//would sort it. If it's not the case things will break because
		//SetListingView won't do any sorting.
		assert(std::equal(parContent.begin(), parContent.end(), SetListing(std::vector<FileRecordData>(parContent)).sorted_list().begin()));

		//TODO: assert that the first item in the list is the shortest string
		std::shared_ptr<PathName> pathname(new PathName(parContent.front().abs_path));
		ConstSetListingView view(parContent.begin(), parContent.end(), pathname->atom_count(), pathname);
		assert(parContent.size() >= 1);
		return guess_content_type(parMediaType, view, parContent.size() - 1);
	}

	char content_type_to_char (mchlib::ContentTypes parCType) {
		return static_cast<char>(parCType);
	}

	ContentTypes char_to_content_type (char parCType) {
		return static_cast<ContentTypes>(parCType);
	}
} //namespace mchlib

#endif
