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
#include <boost/iterator/filter_iterator.hpp>
#include <cstdint>
#include <algorithm>
#include <functional>
#include <ciso646>
#include <regex>
#include <utility>

namespace mchlib {
	namespace {
		using FoundItemPair = std::pair<bool, ConstSetListingView::const_iterator>;

		template <typename O, uint16_t L>
		struct IsLevelLikeO {
			bool operator() ( const FileRecordData& parEntry );
		};

		struct EntryChecking {
			typedef bool(*CheckerFunction)(dinlib::MediaTypes, const ConstSetListingView&);

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

		bool is_path_eq (const char* parPath, const FileRecordData& parEntry) {
			return (parEntry.path == parPath);
		}

		//std::vector<int> check_missing_content (const ConstSetListingView& parContent, const std::vector<const char*>& parCheck) {
		//	std::vector<int> retval;
		//	for (int z = 0; z < static_cast<int>(parCheck.size()), ++z) {
		//		auto glob_range = glob(parContent, parCheck[z]);
		//		if (boost::empty(glob_range)) {
		//			retval.push_back(z);
		//		}
		//	}
		//	return std::move(retval);
		//}

		FoundItemPair find_item (const ConstSetListingView& parContent, const char* parPath) {
			auto it = std::find_if(
				parContent.begin(),
				parContent.end(),
				[parPath](const FileRecordData& parEntry) {
					return (parEntry.path == parPath);
				}
			);

			return std::make_pair(parContent.end() != it, it);
		}

		bool identify_video_dvd (dinlib::MediaTypes parMediaType, const ConstSetListingView& parContent) {
			if (parMediaType != dinlib::MediaType_DVD) {
				return false;
			}

			const auto items_count = count_listing_items(parContent);
			if (items_count < 2) {
				return false;
			}

			auto it_video_ts = std::find_if(parContent.begin(), parContent.end(), std::bind(&is_path_eq, "VIDEO_TS", std::placeholders::_1));
			if (parContent.end() == it_video_ts) {
				return false;
			}

			auto it_audio_ts = std::find_if(parContent.begin(), parContent.end(), std::bind(&is_path_eq, "AUDIO_TS", std::placeholders::_1));
			if (parContent.end() == it_audio_ts) {
				return false;
			}

			return true;
		}

		bool identify_video_cd (dinlib::MediaTypes parMediaType, const ConstSetListingView& parContent) {
			if (parMediaType != dinlib::MediaType_CDRom)
				return false;

			const auto items_count = count_listing_items(parContent);
			if (items_count < 4)
				return false;

			//const std::vector<const char*> should_have {
			//	"SVCD/*.VCD",
			//	"MPEGAV/AVSEQ??.DAT",
			//	"SEGMENT/ITEM???.DAT",
			//	"CDI"
			//};

			auto found = find_item(parContent, "SVCD");
			if (not found.first or not found.second->is_directory)
				return false;
			found = find_item(parContent, "MPEGAV");
			if (not found.first or not found.second->is_directory)
				return false;
			found = find_item(parContent, "SEGMENT");
			if (not found.first or not found.second->is_directory)
				return false;
			found = find_item(parContent, "CDI");
			if (not found.first or not found.second->is_directory)
				return false;

			//FileRecordData("SVCD",0,0,1,true,false),
			//FileRecordData("SVCD/INFO.VCD",0,0,2,false,false),
			//FileRecordData("SVCD/ENTRIES.VCD",0,0,2,false,false),
			//FileRecordData("SVCD/SEARCH.DAT",0,0,2,false,false),
			//FileRecordData("SVCD/PSD.VCD",0,0,2,false,false),
			//FileRecordData("MPEGAV",0,0,1,true,false),
			//FileRecordData("MPEGAV/AVSEQ01.DAT",0,0,2,false,false),
			//FileRecordData("SEGMENT",0,0,1,true,false),
			//FileRecordData("SEGMENT/ITEM001.DAT",0,0,2,false,false),
			//FileRecordData("CDI",0,0,1,true,false),
			//FileRecordData("KARAOKE",0,0,1,true,false)
			return true;
		}
	} //unnamed namespace

	ContentTypes guess_content_type (dinlib::MediaTypes parMediaType, const ConstSetListingView& parContent, std::size_t parEntriesCount) {
		std::vector<EntryChecking> checker_chain {
			{ 100, &identify_video_dvd, ContentType_VideoDVD },
			{ 200, &identify_video_cd, ContentType_VideoCD }
		};

		const auto total_entries = (parEntriesCount ? parEntriesCount : count_listing_items_recursive(parContent));

		for (const auto& chk : checker_chain) {
			if (chk.max_total_entries and chk.max_total_entries >= total_entries) {
				if (chk.checker_func(parMediaType, parContent)) {
					return chk.content_type;
				}
			}
		}
		return ContentType_Generic;
	}
} //namespace mchlib

#endif
