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
#include <sstream>
#include <tuple>
#include <regex>

namespace dindb {
	namespace {
		std::vector<std::regex> compile_regexes (const std::vector<std::string>& parRegexes) {
			std::vector<std::regex> retval;
			retval.reserve(parRegexes.size());
			for (const auto& str : parRegexes) {
				retval.emplace_back(std::regex(
					str,
					std::regex_constants::optimize | std::regex_constants::nosubs | std::regex_constants::ECMAScript
				));
			}
			assert(retval.size() == parRegexes.size());
			return retval;
		}

		void run_script (redis::Command& parRedis, redis::Script& parScript, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
			using dinhelp::lexical_cast;

			auto batch = parRedis.make_batch();
			const std::string set_key = (parSet != InvalidGroupID ? PROGRAM_NAME ":set:" + lexical_cast<std::string>(parSet) : "");
			for (const auto file_id : parFiles) {
				for (const auto &tag : parTags) {
					std::ostringstream oss;
					oss << PROGRAM_NAME ":tag:" << tag;
					const std::string tag_key = oss.str();
					const std::string file_key = PROGRAM_NAME ":file:" + lexical_cast<std::string>(file_id);
					parScript.run(batch, std::make_tuple(tag_key, file_key), std::make_tuple(set_key));
				}
			}

			batch.throw_if_failed();
		}
	} //unnamed namespace

	void tag_files (redis::Command& parRedis, redis::Script& parTagIfInSet, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		run_script(parRedis, parTagIfInSet, parFiles, parTags, parSet);
	}

	void tag_files (redis::Command& parRedis, redis::Script& parTagIfInSet, const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		using dinhelp::lexical_cast;

		const std::string set_key = (parSet != InvalidGroupID ? PROGRAM_NAME ":set:" + lexical_cast<std::string>(parSet) : "");
		const auto regexes = compile_regexes(parRegexes);
		for (const auto& itm : parRedis.scan(PROGRAM_NAME ":file:*")) {
			const auto& file_key = itm;
			const auto path = redis::get_string(parRedis.run("HGET", file_key, "path"));

			auto batch = parRedis.make_batch();
			for (const auto& regex : regexes) {
				if (not std::regex_search(path, regex))
					continue;

				for (const auto &tag : parTags) {
					std::ostringstream oss;
					oss << PROGRAM_NAME ":tag:" << tag;
					const std::string tag_key = oss.str();
					parTagIfInSet.run(batch, std::make_tuple(tag_key, file_key), std::make_tuple(set_key));
				}
			}
			batch.throw_if_failed();
		}
	}

	void delete_tags (redis::Command& parRedis, redis::Script& parDeleIfInSet, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		run_script(parRedis, parDeleIfInSet, parFiles, parTags, parSet);
	}

	void delete_tags (redis::Command& parRedis, redis::Script& parDeleIfInSet, const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
	}

	void delete_all_tags (redis::Command& parRedis, redis::Script& parDeleIfInSet, const std::vector<uint64_t>& parFiles, GroupIDType parSet) {
	}

	void delete_all_tags (redis::Command& parRedis, redis::Script& parDeleIfInSet, const std::vector<std::string>& parRegexes, GroupIDType parSet) {
	}
} //namespace dindb

