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

#include "tag.hpp"
#include "command.hpp"
#include "dindexerConfig.h"
#include "helpers/lexical_cast.hpp"
#include "dindexer-core/split_tags.hpp"
#include <sstream>
#include <tuple>
#include <boost/regex.hpp>
#include <unordered_set>
#include <boost/functional/hash.hpp>

namespace std {
    template<>
    struct hash<boost::string_ref> {
        size_t operator() (boost::string_ref const& sr) const {
            return boost::hash_range(sr.begin(), sr.end());
        }
    };
} //namespace std

namespace dindb {
	namespace {
		std::string make_file_key (uint64_t parID) {
			return PROGRAM_NAME ":file:" + dinhelp::lexical_cast<std::string>(parID);
		}

		std::vector<boost::regex> compile_regexes (const std::vector<std::string>& parRegexes) {
			std::vector<boost::regex> retval;
			retval.reserve(parRegexes.size());
			for (const auto& str : parRegexes) {
				retval.emplace_back(boost::regex(
					str,
					boost::regex_constants::optimize | boost::regex_constants::nosubs | boost::regex_constants::perl
				));
			}
			assert(retval.size() == parRegexes.size());
			return retval;
		}

		void run_id_based_script (redis::Command& parRedis, redis::Script& parScript, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
			using dinhelp::lexical_cast;

			auto batch = parRedis.make_batch();
			const std::string set_key = (parSet != InvalidGroupID ? PROGRAM_NAME ":set:" + lexical_cast<std::string>(parSet) : "");
			for (const auto file_id : parFiles) {
				for (const auto &tag : parTags) {
					std::ostringstream oss;
					oss << PROGRAM_NAME ":tag:" << tag;
					const std::string tag_key = oss.str();
					const std::string file_key = make_file_key(file_id);
					parScript.run(batch, std::make_tuple(tag_key, file_key), std::make_tuple(set_key));
				}
			}

			batch.throw_if_failed();
		}

		void run_regex_based_script(redis::Command& parRedis, redis::Script& parTagIfInSet, const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
			using dinhelp::lexical_cast;

			const std::string set_key = (parSet != InvalidGroupID ? PROGRAM_NAME ":set:" + lexical_cast<std::string>(parSet) : "");
			const auto regexes = compile_regexes(parRegexes);
			for (const auto &itm : parRedis.scan(PROGRAM_NAME ":file:*")) {
				const auto &file_key = itm;
				const auto path = redis::get_string(parRedis.run("HGET", file_key, "path"));

				auto batch = parRedis.make_batch();
				for (const auto &regex : regexes) {
					if (not boost::regex_search(path, regex))
						continue;

					for (const auto &tag : parTags) {
						std::ostringstream oss;
						oss << PROGRAM_NAME ":tag:" << tag;
						const std::string tag_key = oss.str();
						parTagIfInSet.run(batch, std::make_tuple(tag_key, file_key), std::make_tuple(set_key));
					}
					break;
				}
				batch.throw_if_failed();
			}
		}

		template <typename T>
		T id_from_redis_key (const std::string& parKey) {
			assert(not parKey.empty());
			return dinhelp::lexical_cast<T>(dincore::split_and_trim(parKey, ':').back());
		}
	} //unnamed namespace

	void tag_files (redis::Command& parRedis, redis::Script& parTagIfInSet, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		run_id_based_script(parRedis, parTagIfInSet, parFiles, parTags, parSet);
	}

	void tag_files (redis::Command& parRedis, redis::Script& parTagIfInSet, const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		run_regex_based_script(parRedis, parTagIfInSet, parRegexes, parTags, parSet);
	}

	void delete_tags (redis::Command& parRedis, redis::Script& parDeleIfInSet, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		run_id_based_script(parRedis, parDeleIfInSet, parFiles, parTags, parSet);
	}

	void delete_tags (redis::Command& parRedis, redis::Script& parDeleIfInSet, const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		run_regex_based_script(parRedis, parDeleIfInSet, parRegexes, parTags, parSet);
	}

	void delete_all_tags (redis::Command& parRedis, redis::Script& parDeleIfInSet, const std::vector<uint64_t>& parFiles, GroupIDType parSet) {
		auto batch = parRedis.make_batch();
		for (const auto file_id : parFiles) {
			const auto file_key = make_file_key(file_id);
			batch.run("HGET", file_key, "tags");
		}

		batch.throw_if_failed();
		std::unordered_set<boost::string_ref> dele_tags;
		for (const auto& reply : batch.replies()) {
			auto tags = dincore::split_tags(redis::get_string(reply));
			for (const auto& tag : tags) {
				dele_tags.insert(tag);
			}
		}

		std::vector<boost::string_ref> vec_dele_tags(dele_tags.begin(), dele_tags.end());
		delete_tags(parRedis, parDeleIfInSet, parFiles, vec_dele_tags, parSet);
	}

	void delete_all_tags (redis::Command& parRedis, redis::Script& parDeleIfInSet, const std::vector<std::string>& parRegexes, GroupIDType parSet) {
		using dinhelp::lexical_cast;

		const std::string set_key = (parSet != InvalidGroupID ? PROGRAM_NAME ":set:" + lexical_cast<std::string>(parSet) : "");
		const auto regexes = compile_regexes(parRegexes);

		std::set<std::string> dele_tags;
		std::vector<uint64_t> ids;

		for (const auto& itm : parRedis.scan(PROGRAM_NAME ":file:*")) {
			const auto& file_key = itm;
			auto file_reply = parRedis.run("HMGET", file_key, "path", "tags", "group_id");
			auto& file_replies = redis::get_array(file_reply);
			assert(file_replies.size() == 3);
			const auto group_id = id_from_redis_key<GroupIDType>(redis::get_string(file_replies[2]));
			if (parSet != InvalidGroupID and parSet != group_id)
				continue;

			const auto path = redis::get_string(file_replies[0]);
			const auto tags_str = (file_replies[1].which() == redis::RedisVariantType_Nil ? std::string() : redis::get_string(file_replies[1]));
			const auto tags = dincore::split_tags(tags_str);
			const auto file_id = id_from_redis_key<FileIDType>(file_key);

			for (const auto &regex : regexes) {
				if (not boost::regex_search(path, regex))
					continue;

				ids.push_back(file_id);

				for (const auto &tag : tags) {
					dele_tags.insert(std::string(tag.data(), tag.size()));
				}
				break;
			}
		}

		std::vector<boost::string_ref> dele_tags_vec;
		dele_tags_vec.reserve(dele_tags.size());
		std::transform(
			dele_tags.begin(),
			dele_tags.end(),
			std::back_inserter(dele_tags_vec),
			[](const std::string& parS) { return boost::string_ref(parS); }
		);

		delete_tags(parRedis, parDeleIfInSet, ids, dele_tags_vec, parSet);
	}
} //namespace dindb

