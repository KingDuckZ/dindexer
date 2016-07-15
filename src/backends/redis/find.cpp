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
#include "incredis/incredis.hpp"
#include "duckhandy/lexical_cast.hpp"
#include "dindexerConfig.h"
#include "dindexer-core/split_tags.hpp"
#include "dindexer-machinery/tiger.hpp"
#include "duckhandy/compatibility.h"
#include <boost/regex.hpp>
#include <ciso646>
#include <algorithm>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <functional>
#include <iterator>
#include <cstdint>
#include <boost/algorithm/string/predicate.hpp>

namespace dindb {
	namespace {
		inline std::string to_std_string ( boost::string_ref parStr ) a_always_inline;
		template <typename T>
		inline void concatenate ( std::vector<T>&& parAppend, std::vector<T>& parOut ) a_always_inline;
		template <typename T>
		inline T construct ( const std::vector<redis::Reply>& parData, const std::string& parID ) a_always_inline;
		template <>
		inline LocatedItem construct ( const std::vector<redis::Reply>& parData, const std::string& parID ) a_always_inline;
		template <>
		inline LocatedSet construct ( const std::vector<redis::Reply>& parData, const std::string& parID ) a_always_inline;

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

		template <typename T>
		std::vector<T> store_filtered_items (
			const std::vector<redis::Reply>& parReplies,
			const std::vector<std::string>& parIDs,
			std::function<bool(const boost::tuple<std::vector<redis::Reply>, std::string>&)> parFilter
		) {
			using boost::adaptors::filtered;
			using boost::adaptors::transformed;
			using boost::tuple;
			using boost::make_tuple;
			using redis::Reply;
			using std::vector;
			using dhandy::lexical_cast;

			assert(parReplies.size() == parIDs.size());
			return boost::copy_range<vector<T>>(
				zip_range(parReplies, parIDs) |
				transformed([](const tuple<Reply, std::string>& r) {
					return make_tuple(redis::get_array(r.get<0>()), r.get<1>());
				}) |
				filtered(parFilter) |
				transformed([](const tuple<vector<Reply>, std::string>& t) {
					return construct<T>(t.get<0>(), t.get<1>());
				})
			);
		}

		std::string to_std_string (boost::string_ref parStr) {
			return std::string(parStr.data(), parStr.size());
		}

		template <typename T>
		void concatenate (std::vector<T>&& parAppend, std::vector<T>& parOut) {
			parOut.insert(parOut.end(), std::make_move_iterator(parAppend.begin()), std::make_move_iterator(parAppend.end()));
		}

		template <>
		LocatedItem construct (const std::vector<redis::Reply>& parData, const std::string& parID) {
			using dhandy::lexical_cast;

			//parData is expected to contain: "path", "group_id", "tags"

			assert(parData.size() == 3);
			return LocatedItem{
				redis::get_string(parData[0]),
				lexical_cast<FileIDType>(parID),
				lexical_cast<GroupIDType>(redis::get_string(parData[1]))
			};
		}

		template <>
		LocatedSet construct (const std::vector<redis::Reply>& parData, const std::string& parID) {
			using dhandy::lexical_cast;

			//parData is expected to contain: "desc", "item_count", "dir_count"

			assert(parData.size() == 3);
			const auto itm_count = lexical_cast<uint32_t>(redis::get_string(parData[1]));
			const auto dir_count = lexical_cast<uint32_t>(redis::get_string(parData[2]));
			assert(dir_count <= itm_count);

			return LocatedSet{
				redis::get_string(parData[0]),
				lexical_cast<GroupIDType>(parID),
				itm_count - dir_count,
				dir_count
			};
		}

		template <typename T, typename F, typename... FIELDS>
		std::vector<T> locate_in_bursts (redis::IncRedis& parRedis, const char* parScanKeyFilter, F parFilter, FIELDS&&... parFields) {
			using dincore::split_and_trim;

			const int prefetch_count = 500;
			std::vector<T> retval;
			std::vector<std::string> ids;
			ids.reserve(prefetch_count);

			int curr_count = 0;
			auto batch = parRedis.make_batch();
			for (const auto& itm : parRedis.scan(parScanKeyFilter)) {
				++curr_count;
				batch.hmget(itm, std::forward<FIELDS>(parFields)...);
				ids.push_back(to_std_string(split_and_trim(itm, ':').back()));

				if (curr_count == prefetch_count) {
					concatenate(store_filtered_items<T>(batch.replies(), ids, parFilter), retval);
					batch.reset();
					curr_count = 0;
					ids.clear();
				}
			}
			if (curr_count)
				concatenate(store_filtered_items<T>(batch.replies(), ids, parFilter), retval);
			return retval;
		}
	} //unnamed namespace

	std::vector<GroupIDType> find_all_sets (redis::IncRedis& parRedis) {
		using dincore::split_and_trim;

		std::vector<GroupIDType> retval;
		for (const auto& itm : parRedis.scan(PROGRAM_NAME ":set:*")) {
			retval.push_back(dhandy::lexical_cast<GroupIDType>(split_and_trim(itm, ':').back()));
		}
		return retval;
	}

	std::vector<LocatedItem> locate_in_db (redis::IncRedis& parRedis, const std::string& parRegex, const TagList& parTags) {
		const boost::regex search(parRegex, boost::regex_constants::optimize | boost::regex_constants::nosubs | boost::regex_constants::perl);

		auto filter = [&parTags, &search](const boost::tuple<std::vector<redis::Reply>, std::string>& t) {
			return (parTags.empty() or all_tags_match(parTags, redis::get_string(t.get<0>()[2]))) and
				boost::regex_search(redis::get_string(t.get<0>()[0]), search);
		};

		return locate_in_bursts<LocatedItem>(parRedis, PROGRAM_NAME ":file:*", filter, "path", "group_id", "tags");
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

			return store_filtered_items<LocatedItem>(
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

	std::vector<LocatedSet> locate_sets_in_db (redis::IncRedis& parRedis, const std::string& parSubstr, bool parCaseInsensitive) {
		return locate_sets_in_db(parRedis, parSubstr, std::vector<GroupIDType>(), parCaseInsensitive);
	}

	std::vector<LocatedSet> locate_sets_in_db (redis::IncRedis& parRedis, const std::string& parSubstr, const std::vector<GroupIDType>& parSets, bool parCaseInsensitive) {
		using dhandy::lexical_cast;

		auto filter_case_ins = [&parSubstr, &parSets](const boost::tuple<std::vector<redis::Reply>, std::string>& t) {
			const auto& s = redis::get_string(t.get<0>()[0]);
			return s.end() != std::search(
				s.begin(),
				s.end(),
				parSubstr.begin(),
				parSubstr.end(),
				[](char c1, char c2) {return std::toupper(c1) == std::toupper(c2);}) and
				(
					parSets.empty() or
					std::find(parSets.begin(), parSets.end(), lexical_cast<GroupIDType>(t.get<1>())) != parSets.end()
				);
		};
		auto filter_case_sens = [&parSubstr, &parSets](const boost::tuple<std::vector<redis::Reply>, std::string>& t) {
			return redis::get_string(t.get<0>()[0]).find(parSubstr) != std::string::npos and
				(
					parSets.empty() or
					std::find(parSets.begin(), parSets.end(), lexical_cast<GroupIDType>(t.get<1>())) != parSets.end()
				);
		};
		std::function<bool(const boost::tuple<std::vector<redis::Reply>, std::string>&)> filter;
		if (parCaseInsensitive)
			filter = filter_case_ins;
		else
			filter = filter_case_sens;

		return locate_in_bursts<LocatedSet>(parRedis, PROGRAM_NAME ":set:*", filter, "desc", "item_count", "dir_count");
	}

	std::vector<dhandy::MaxSizedArray<std::string, 4>> find_set_details (redis::IncRedis& parRedis, const std::vector<GroupIDType>& parSets) {
		using dhandy::lexical_cast;

		auto batch = parRedis.make_batch();
		for (auto set_id : parSets) {
			const auto set_key = PROGRAM_NAME ":set:" + lexical_cast<std::string>(set_id);
			batch.hmget(set_key, "creation", "name", "disk_label");
		}
		batch.throw_if_failed();

		std::vector<dhandy::MaxSizedArray<std::string, 4>> retval;
		auto curr_set = parSets.begin();
		for (const auto& reply : batch.replies()) {
			const auto& reply_list = get_array(reply);
			if (redis::RedisVariantType_Nil != reply_list[0].which() and
				redis::RedisVariantType_Nil != reply_list[1].which() and
				redis::RedisVariantType_Nil != reply_list[2].which())
			{
				retval.resize(retval.size() + 1);
				retval.back().push_back(lexical_cast<std::string>(*curr_set));
				retval.back().push_back(get_string(reply_list[1]));
				retval.back().push_back(get_string(reply_list[0]));
				retval.back().push_back(get_string(reply_list[2]));
			}
			++curr_set;
		}
		return retval;
	};

	std::vector<dhandy::MaxSizedArray<std::string, 1>> find_file_details (redis::IncRedis& parRedis, GroupIDType parSetID, uint16_t parLevel, boost::string_ref parDir) {
		using dhandy::lexical_cast;
		using RetListType = std::vector<dhandy::MaxSizedArray<std::string, 1>>;

		const double level = static_cast<float>(parLevel);
		auto lst = parRedis.zrangebyscore(PROGRAM_NAME ":levels:" + lexical_cast<std::string>(parSetID), level, true, level, true, false);
		if (not lst)
			return RetListType();

		auto batch = parRedis.make_batch();
		for (const auto& itm : *lst) {
			if (itm)
				batch.hget(*itm, "path");
		}
		batch.throw_if_failed();

		std::vector<dhandy::MaxSizedArray<std::string, 1>> retval;
		for (auto& reply : batch.replies()) {
			if (redis::RedisVariantType_Nil != reply.which()) {
				auto curr_path = get_string(reply);
				if (boost::starts_with(curr_path, parDir)) {
					retval.resize(retval.size() + 1);
					retval.back().push_back(std::move(curr_path));
				}
			}
		}
		return retval;
	};

	std::vector<std::string> find_paths_starting_by (redis::IncRedis& parRedis, GroupIDType parGroupID, uint16_t parLevel, boost::string_ref parPath) {
		using boost::adaptors::transformed;
		using dhandy::MaxSizedArray;

		auto file_details = find_file_details(parRedis, parGroupID, parLevel, parPath);
		return boost::copy_range<std::vector<std::string>>(
			file_details |
			transformed([](MaxSizedArray<std::string, 1>& a){return std::move(a.front());})
		);
	}
} //namespace dindb
