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
	} //unnamed namespace

	void tag_files (redis::Command& parRedis, redis::Script& parTagIfInSet, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		using dinhelp::lexical_cast;

		auto batch = parRedis.make_batch();
		const std::string set_key = (parSet != InvalidGroupID ? PROGRAM_NAME ":set:" + lexical_cast<std::string>(parSet) : "");
		for (const auto file_id : parFiles) {
			for (const auto &tag : parTags) {
				std::ostringstream oss;
				oss << PROGRAM_NAME ":tag:" << tag;
				const std::string tag_key = oss.str();
				const std::string file_key = PROGRAM_NAME ":file:" + lexical_cast<std::string>(file_id);
				parTagIfInSet.run(batch, std::make_tuple(tag_key, file_key), std::make_tuple(set_key));
			}
		}

		batch.throw_if_failed();
	}

	void tag_files (redis::Command& parRedis, redis::Script& parTagIfInSet, const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
	}

	void delete_tags (redis::Command& parRedis, redis::Script& parDeleIfInSet, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
	}

	void delete_tags (redis::Command& parRedis, redis::Script& parDeleIfInSet, const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
	}

	void delete_all_tags (redis::Command& parRedis, redis::Script& parDeleIfInSet, const std::vector<uint64_t>& parFiles, GroupIDType parSet) {
	}

	void delete_all_tags (redis::Command& parRedis, redis::Script& parDeleIfInSet, const std::vector<std::string>& parRegexes, GroupIDType parSet) {
	}
} //namespace dindb

