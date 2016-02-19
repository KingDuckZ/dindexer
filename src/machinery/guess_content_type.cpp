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

#include "guess_content_type.hpp"
#include "dindexer-machinery/set_listing.hpp"
#include "dindexer-machinery/set_listing_helpers.hpp"
#include <boost/iterator/filter_iterator.hpp>
#include <cstdint>
#include <algorithm>
#include <functional>
#include <ciso646>
#include <regex>

namespace mchlib {
	namespace {
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
	} //unnamed namespace

	ContentTypes guess_content_type (dinlib::MediaTypes parMediaType, const ConstSetListingView& parContent) {
		std::vector<EntryChecking> checker_chain {
			{ 100, &identify_video_dvd, ContentType_VideoDVD }
		};

		assert(false);
		const auto total_entries = count_listing_items_recursive(parContent);

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
