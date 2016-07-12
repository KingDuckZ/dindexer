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

#include "delete.hpp"
#include "tag.hpp"
#include "incredis.hpp"
#include "helpers/lexical_cast.hpp"
#include "helpers/sequence_bt.hpp"
#include "dindexerConfig.h"
#include <vector>
#include <tuple>
#include <utility>
#include <boost/iterator/counting_iterator.hpp>
#include <algorithm>

namespace dindb {
	namespace {
		std::pair<bool, std::size_t> confirm_dele (redis::IncRedisBatch& parBatch, const std::vector<GroupIDType>& parIDs, ConfirmDeleCallback parConf) {
			using dinhelp::lexical_cast;

			if (parIDs.empty())
				return std::make_pair(false, parIDs.size());

			for (auto id : parIDs) {
				const auto set_key = PROGRAM_NAME ":set:" + lexical_cast<std::string>(id);
				parBatch.hmget(set_key, "base_file_id", "file_count", "name");
			}

			std::map<GroupIDType, std::string> set_dele_list;
			std::size_t id_index = 0;
			for (const auto& reply : parBatch.replies()) {
				const auto res = redis::get_array(reply);
				assert(res.size() == 3);
				if (redis::RedisVariantType_Nil != res[0].which()) {
					assert(redis::RedisVariantType_Nil != res[1].which());
					set_dele_list[parIDs[id_index]] = redis::get_string(res[2]);
				}
				++id_index;
			}

			if (set_dele_list.empty())
				return std::make_pair(false, set_dele_list.size());
			else
				return std::make_pair(parConf(set_dele_list), set_dele_list.size());
				//return std::make_pair(true, set_dele_list.size());
		}

		template <typename IT, IT CHUNK, typename F, IT... SVALS>
		void chunked_run_pvt (redis::Batch& parBatch, const char* parCommand, IT parFrom, IT parCount, F parMakeKey, dinhelp::bt::number_seq<IT, SVALS...>) {
			for (IT i = 0; i < parCount / CHUNK; ++i) {
				parBatch.run(parCommand, parMakeKey(i * CHUNK + parFrom + SVALS)...);
			}
			for (auto i = ((parFrom + parCount) / CHUNK) * CHUNK; i < parFrom + parCount; ++i) {
				const auto key = parMakeKey(i);
				parBatch.run(parCommand, key);
			}
		}

		template <typename IT, IT CHUNK, typename F>
		void chunked_run (redis::Batch& parBatch, const char* parCommand, IT parFrom, IT parCount, F parMakeKey) {
			chunked_run_pvt<IT, CHUNK, F>(parBatch, parCommand, parFrom, parCount, parMakeKey, dinhelp::bt::number_range<IT, 0, CHUNK>());
		};
	} //unnamed namespace

	void delete_group_from_db (redis::IncRedis& parRedis, redis::Script& parDeleTagIfInSet, redis::Script& parDeleHash, const std::vector<GroupIDType>& parIDs, ConfirmDeleCallback parConf) {
		using dinhelp::lexical_cast;
		using IDRange = std::tuple<GroupIDType, FileIDType, FileIDType>;

		auto set_batch = parRedis.make_batch();

		auto dele_pair = confirm_dele(set_batch, parIDs, parConf);
		assert(set_batch.batch().replies_requested());
		if (not dele_pair.first)
			return;

		std::vector<IDRange> ranges;
		ranges.reserve(dele_pair.second);
		std::size_t id_index = 0;
		for (const auto& reply : set_batch.replies()) {
			const auto res = redis::get_array(reply);
			if (redis::RedisVariantType_Nil != res[0].which()) {
				ranges.push_back(
					IDRange(
						parIDs[id_index],
						lexical_cast<FileIDType>(redis::get_string(res[0])),
						lexical_cast<FileIDType>(redis::get_string(res[1]))
					)
				);
			}
			++id_index;
		}
		assert(ranges.size() == dele_pair.second);

		for (const auto& dele_tuple : ranges) {
			const auto set_id = std::get<0>(dele_tuple);
			const auto file_base_index = std::get<1>(dele_tuple);
			const auto file_count = std::get<2>(dele_tuple);

			std::vector<FileIDType> ids;
			ids.reserve(file_count);
			std::copy(
				boost::counting_iterator<FileIDType>(file_base_index),
				boost::counting_iterator<FileIDType>(file_base_index + file_count),
				std::back_inserter(ids)
			);

			delete_all_tags(parRedis, parDeleTagIfInSet, ids, set_id);
		}

		auto dele_batch = parRedis.make_batch();
		for (const auto& dele_tuple : ranges) {
			const auto set_id = std::get<0>(dele_tuple);
			const auto file_base_index = std::get<1>(dele_tuple);
			const auto file_count = std::get<2>(dele_tuple);

			auto hash_query_batch = parRedis.make_batch();
			for (FileIDType i = file_base_index; i < file_base_index + file_count; ++i) {
				const auto file_key = PROGRAM_NAME ":file:" + lexical_cast<std::string>(i);
				hash_query_batch.hget(file_key, "hash");
			}
			hash_query_batch.throw_if_failed();

			for (const auto& rep : hash_query_batch.replies()) {
				const auto hash_key = PROGRAM_NAME ":hash:" + redis::get_string(rep);
				parDeleHash.run(
					dele_batch.batch(),
					std::make_tuple(hash_key),
					std::make_tuple(lexical_cast<std::string>(file_base_index), lexical_cast<std::string>(file_count))
				);
			}

			dele_batch.del(PROGRAM_NAME ":set:" + lexical_cast<std::string>(set_id));
			chunked_run<FileIDType, 8>(dele_batch.batch(), +"DEL", file_base_index, file_count, [](FileIDType id){return PROGRAM_NAME ":file:" + lexical_cast<std::string>(id);});
		}

		dele_batch.throw_if_failed();
	}
} //namespace dindb
