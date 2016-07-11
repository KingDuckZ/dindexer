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

#ifndef id2874EA620CF0415CAF5B005E227BC44B
#define id2874EA620CF0415CAF5B005E227BC44B

#include "backends/db_backend.hpp"
#include <vector>
#include <boost/utility/string_ref.hpp>

namespace redis {
	class Command;
	class Script;
} //namespace redis

namespace dindb {
	void tag_files (
		redis::Command& parRedis,
		redis::Script& parTagIfInSet,
		const std::vector<FileIDType>& parFiles,
		const std::vector<boost::string_ref>& parTags,
		GroupIDType parSet
	);
	void tag_files (
		redis::Command& parRedis,
		redis::Script& parTagIfInSet,
		const std::vector<std::string>& parRegexes,
		const std::vector<boost::string_ref>& parTags,
		GroupIDType parSet
	);

	void delete_tags (
		redis::Command& parRedis,
		redis::Script& parDeleIfInSet,
		const std::vector<FileIDType>& parFiles,
		const std::vector<boost::string_ref>& parTags,
		GroupIDType parSet
	);
	void delete_tags (
		redis::Command& parRedis,
		redis::Script& parDeleIfInSet,
		const std::vector<std::string>& parRegexes,
		const std::vector<boost::string_ref>& parTags,
		GroupIDType parSet
	);
	void delete_all_tags (
		redis::Command& parRedis,
		redis::Script& parDeleIfInSet,
		const std::vector<FileIDType>& parFiles,
		GroupIDType parSet
	);
	void delete_all_tags (
		redis::Command& parRedis,
		redis::Script& parDeleIfInSet,
		const std::vector<std::string>& parRegexes,
		GroupIDType parSet
	);
} //namespace dindb

#endif
