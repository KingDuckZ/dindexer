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
#include "pq/connection.hpp"
#include <ciso646>
#include <cassert>

namespace dindb {
	void tag_files (pq::Connection& parDB, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		assert(parDB.is_connected());

		if (InvalidGroupID != parSet) {
			const std::string query =
				"UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"id\"=ANY($2) AND \"group_id\"=$3;";
			parDB.query(query, parTags, parFiles, parSet);
		}
		else {
			const std::string query =
				"UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"id\"=ANY($2);";
			parDB.query(query, parTags, parFiles);
		}
	}

	void tag_files (pq::Connection& parDB, const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		assert(parDB.is_connected());

		if (InvalidGroupID != parSet) {
			if (parRegexes.size() == 1) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"group_id\"=$2 AND \"path\" ~ $3;";
				parDB.query(query, parTags, parSet, parRegexes.front());
			}
			else if (parRegexes.size() > 1) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"group_id\"=$2 AND \"path\" ~ ANY($3);";
				parDB.query(query, parTags, parSet, parRegexes);
			}
			else if (parRegexes.size() == 0) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) WHERE \"group_id\"=$2 ORDER BY 1);";
				parDB.query(query, parTags, parSet);
			}
		}
		else {
			if (parRegexes.size() == 1) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"path\" ~ $2;";
				parDB.query(query, parTags, parRegexes.front());
			}
			else if (parRegexes.size() > 1) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"path\" ~ ANY($2);";
				parDB.query(query, parTags, parRegexes);
			}
			else if (parRegexes.size() == 0) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1);";
				parDB.query(query, parTags);
			}
		}
	}

	void delete_tags (pq::Connection& parDB, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		assert(parDB.is_connected());

		if (parTags.size() == 1) {
			if (InvalidGroupID != parSet) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY_REMOVE(tags, $1) WHERE \"id\" = ANY($2) AND \"group_id\" = $3;";
				parDB.query(query, parTags.front(), parFiles, parSet);
			}
			else {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY_REMOVE(tags, $1) WHERE \"id\" = ANY($2);";
				parDB.query(query, parTags.front(), parFiles);
			}
		}
		else {
			if (InvalidGroupID != parSet) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT UNNEST(\"tags\") EXCEPT SELECT UNNEST($1)) WHERE \"id\" = ANY($2) AND \"group_id\" = $3;";
				parDB.query(query, parTags, parFiles, parSet);
			}
			else {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT UNNEST(\"tags\") EXCEPT SELECT UNNEST($1)) WHERE \"id\" = ANY($2);";
				parDB.query(query, parTags, parFiles);
			}
		}
	}

	void delete_tags (pq::Connection& parDB, const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		assert(parDB.is_connected());

		if (parTags.size() == 1) {
			if (InvalidGroupID != parSet) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY_REMOVE(tags, $1) WHERE  \"group_id\" = $3 AND \"path\" ~ ANY($3);";
				parDB.query(query, parTags.front(), parSet, parRegexes);
			}
			else {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY_REMOVE(tags, $1) WHERE \"path\" ~ ANY($2);";
				parDB.query(query, parTags.front(), parRegexes);
			}
		}
		else {
			if (InvalidGroupID != parSet) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT UNNEST(\"tags\") EXCEPT SELECT UNNEST($1)) WHERE \"group_id\" = $2 AND \"path\" ~ ANY($3);";
				parDB.query(query, parTags, parSet, parRegexes);
			}
			else {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT UNNEST(\"tags\") EXCEPT SELECT UNNEST($1)) WHERE \"path\" = ANY($2);";
				parDB.query(query, parTags, parRegexes);
			}
		}
	}

	void delete_all_tags (pq::Connection& parDB, const std::vector<uint64_t>& parFiles, GroupIDType parSet) {
		assert(parDB.is_connected());

		if (InvalidGroupID != parSet) {
			const std::string query =
				"UPDATE \"files\" SET \"tags\" = '{}' WHERE \"id\"=ANY($1) AND \"group_id\"=$2;";
			parDB.query(query, parFiles, parSet);
		}
		else {
			const std::string query =
				"UPDATE \"files\" SET \"tags\" = '{}' WHERE \"id\"=ANY($1);";
			parDB.query(query, parFiles);
		}
	}

	void delete_all_tags (pq::Connection& parDB, const std::vector<std::string>& parRegexes, GroupIDType parSet) {
		assert(parDB.is_connected());

		if (InvalidGroupID != parSet) {
			const std::string query = "UPDATE \"files\" SET \"tags\" = '{}' WHERE \"group_id\"=$1 AND \"path\" ~ ANY($2);";
			parDB.query(query, parSet, parRegexes);
		}
		else {
			const std::string query = "UPDATE \"files\" SET \"tags\" = '{}' WHERE \"path\" ~ ANY($2);";
			parDB.query(query, parRegexes);
		}
	}
} //namespace dindb
