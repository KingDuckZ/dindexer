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

#include "find.hpp"
#include "command.hpp"
#include "helpers/lexical_cast.hpp"
#include "dindexerConfig.h"
#include "dindexer-core/split_tags.hpp"
#include <boost/regex.hpp>
#include <ciso646>
#include <algorithm>

namespace dindb {
	namespace {
		bool all_tags_match (const TagList& parTags, const std::string& parTaglist) {
			const auto tags = dincore::split_tags(parTaglist);

			if (tags.size() >= parTags.size()) {
				for (const auto& required_tag : parTags) {
					if (std::find(tags.begin(), tags.end(), required_tag) == tags.end()) {
						return false;
					}
				}
			}
			else {
				return false;
			}
			return true;
		}

		void store_matching_paths (redis::Batch& parBatch, std::vector<LocatedItem>& parOut, std::vector<FileIDType>& parIDs, const boost::regex& parSearch, const TagList& parTags) {
			using dinhelp::lexical_cast;
			assert(parIDs.size() == parBatch.replies().size());

			parBatch.throw_if_failed();
			std::size_t id_index = 0;
			for (const auto& itm : parBatch.replies()) {
				const auto reply = redis::get_array(itm);
				const auto& path = redis::get_string(reply[0]);

				if (boost::regex_search(path, parSearch)) {
					if (parTags.empty() or all_tags_match(parTags, redis::get_string(reply[2]))) {
						const auto group_id = lexical_cast<GroupIDType>(redis::get_string(reply[1]));
						parOut.push_back(LocatedItem{path, parIDs[id_index], group_id});
					}
				}
				assert(id_index < parIDs.size());
				++id_index;
			}
		}
	} //unnamed namespace

	std::vector<GroupIDType> find_all_sets (redis::Command& parRedis) {
		using dincore::split_and_trim;
		using dinhelp::lexical_cast;

		std::vector<GroupIDType> retval;
		for (const auto& itm : parRedis.scan(PROGRAM_NAME ":set:*")) {
			retval.push_back(lexical_cast<GroupIDType>(split_and_trim(itm, ':').back()));
		}
		return retval;
	}

	std::vector<LocatedItem> locate_in_db (redis::Command& parRedis, const std::string& parSearch, const TagList& parTags) {
		using dincore::split_and_trim;
		using dinhelp::lexical_cast;

		const boost::regex search(parSearch, boost::regex_constants::optimize | boost::regex_constants::nosubs | boost::regex_constants::perl);
		const int prefetch_count = 500;

		std::vector<LocatedItem> retval;
		std::vector<FileIDType> ids;
		ids.reserve(prefetch_count);

		int curr_count = 0;
		auto batch = parRedis.make_batch();
		for (const auto& itm : parRedis.scan(PROGRAM_NAME ":file:*")) {
			++curr_count;
			batch.run("HMGET", itm, "path", "group_id", "tags");
			ids.push_back(lexical_cast<FileIDType>(split_and_trim(itm, ':').back()));

			if (curr_count == prefetch_count) {
				store_matching_paths(batch, retval, ids, search, parTags);
				batch.reset();
				curr_count = 0;
				ids.clear();
			}
		}
		if (curr_count)
			store_matching_paths(batch, retval, ids, search, parTags);
		return retval;
	}
} //namespace dindb
