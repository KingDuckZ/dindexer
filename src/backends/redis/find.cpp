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
#include "incredis.hpp"
#include "helpers/lexical_cast.hpp"
#include "dindexerConfig.h"
#include "dindexer-core/split_tags.hpp"
#include "dindexer-machinery/tiger.hpp"
#include "helpers/compatibility.h"
#include <boost/regex.hpp>
#include <ciso646>
#include <algorithm>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <functional>
#include <iterator>

namespace dindb {
	namespace {
		inline std::string to_std_string ( boost::string_ref parStr ) a_always_inline;
		inline void concatenate ( std::vector<LocatedItem>&& parAppend, std::vector<LocatedItem>& parOut ) a_always_inline;

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

		//See: http://stackoverflow.com/questions/12552277/whats-the-best-way-to-iterate-over-two-or-more-containers-simultaneously/12553437#12553437
		//(referenced from http://stackoverflow.com/questions/16982190/c-use-boost-range-transformed-adaptor-with-binary-function)
		//What became of this? http://marc.info/?l=boost-users&m=129619765731342
		template <typename... Conts>
		auto zip_range (Conts&... parConts) -> decltype(boost::make_iterator_range(
			boost::make_zip_iterator(boost::make_tuple(parConts.begin()...)),
		    boost::make_zip_iterator(boost::make_tuple(parConts.end()...)))
		) {
			return {
				boost::make_zip_iterator(boost::make_tuple(parConts.begin()...)),
				boost::make_zip_iterator(boost::make_tuple(parConts.end()...))
			};
		}

		std::vector<LocatedItem> store_filtered_items (
			const std::vector<redis::Reply>& parReplies,
			const std::vector<std::string>& parIDs,
			std::function<bool(const boost::tuple<std::vector<redis::Reply>, std::string>&)> parFilter
		) {
			using boost::adaptors::filtered;
			using boost::adaptors::transformed;
			using boost::tuple;
			using boost::make_tuple;
			using redis::get_string;
			using redis::Reply;
			using std::vector;
			using dinhelp::lexical_cast;

			assert(parReplies.size() == parIDs.size());
			return boost::copy_range<vector<LocatedItem>>(
					zip_range(parReplies, parIDs) |
							transformed([](const tuple<Reply, std::string>& r) {
								return make_tuple(redis::get_array(r.get<0>()), r.get<1>());
							}) |
							filtered(parFilter) |
							transformed([](const tuple<vector<Reply>, std::string>& t) {
								return LocatedItem{
									get_string(t.get<0>()[0]),
									lexical_cast<FileIDType>(t.get<1>()),
									lexical_cast<GroupIDType>(get_string(t.get<0>()[1]))
								};
							})
			);
		}

		std::string to_std_string (boost::string_ref parStr) {
			return std::string(parStr.data(), parStr.size());
		}

		void concatenate (std::vector<LocatedItem>&& parAppend, std::vector<LocatedItem>& parOut) {
			parOut.insert(parOut.end(), std::make_move_iterator(parAppend.begin()), std::make_move_iterator(parAppend.end()));
		}
	} //unnamed namespace

	std::vector<GroupIDType> find_all_sets (redis::IncRedis& parRedis) {
		using dincore::split_and_trim;
		using dinhelp::lexical_cast;

		std::vector<GroupIDType> retval;
		for (const auto& itm : parRedis.scan(PROGRAM_NAME ":set:*")) {
			retval.push_back(lexical_cast<GroupIDType>(split_and_trim(itm, ':').back()));
		}
		return retval;
	}

	std::vector<LocatedItem> locate_in_db (redis::IncRedis& parRedis, const std::string& parSearch, const TagList& parTags) {
		using dincore::split_and_trim;
		using dinhelp::lexical_cast;

		const boost::regex search(parSearch, boost::regex_constants::optimize | boost::regex_constants::nosubs | boost::regex_constants::perl);
		const int prefetch_count = 500;

		std::vector<LocatedItem> retval;
		std::vector<std::string> ids;
		ids.reserve(prefetch_count);

		auto filter = [&parTags, &search](const boost::tuple<std::vector<redis::Reply>, std::string>& t) {
			return (parTags.empty() or all_tags_match(parTags, redis::get_string(t.get<0>()[2]))) and
				boost::regex_search(redis::get_string(t.get<0>()[0]), search);
		};

		int curr_count = 0;
		auto batch = parRedis.make_batch();
		for (const auto& itm : parRedis.scan(PROGRAM_NAME ":file:*")) {
			++curr_count;
			batch.hmget(itm, "path", "group_id", "tags");
			ids.push_back(to_std_string(split_and_trim(itm, ':').back()));

			if (curr_count == prefetch_count) {
				concatenate(store_filtered_items(batch.replies(), ids, filter), retval);
				batch.reset();
				curr_count = 0;
				ids.clear();
			}
		}
		if (curr_count)
			concatenate(store_filtered_items(batch.replies(), ids, filter), retval);

		return retval;
	}

	std::vector<LocatedItem> locate_in_db (redis::IncRedis& parRedis, const mchlib::TigerHash& parSearch, const TagList& parTags) {
		const auto hash_key = PROGRAM_NAME ":hash:" + mchlib::tiger_to_string(parSearch, false);
		const auto file_ids = parRedis.smembers(hash_key);

		std::vector<std::string> ids;
		if (file_ids) {
			auto batch = parRedis.make_batch();
			for (auto&& file_id : *file_ids) {
				if (not file_id)
					continue;

				const auto file_key = PROGRAM_NAME ":file:" + *file_id;
				ids.emplace_back(std::move(*file_id));
				batch.hmget(file_key, "path", "group_id", "tags");
			}
			batch.throw_if_failed();

			return store_filtered_items(
				batch.replies(),
				ids,
				[&parTags](const boost::tuple<std::vector<redis::Reply>, std::string>& t) {
					return parTags.empty() or all_tags_match(parTags, redis::get_string(t.get<0>()[2]));
				}
			);
		}
		else {
			return std::vector<LocatedItem>();
		}
	}
} //namespace dindb
