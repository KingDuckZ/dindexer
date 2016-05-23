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

#include "backends/postgresql/tag.hpp"
#include "backends/postgresql/settings.hpp"
#include "pq/connection.hpp"
#include "dindexer-common/settings.hpp"
#include <ciso646>

namespace dinbpostgres {
	void tag_files (const Settings& parDB, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags, OwnerSetInfo parSet) {
		pq::Connection conn(std::string(parDB.username), std::string(parDB.password), std::string(parDB.dbname), std::string(parDB.address), parDB.port);
		conn.connect();

		if (parSet.is_valid) {
			const std::string query =
				"UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"id\"=ANY($2) AND \"group_id\"=$3;";
			conn.query(query, parTags, parFiles, parSet.group_id);
		}
		else {
			const std::string query =
				"UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"id\"=ANY($2);";
			conn.query(query, parTags, parFiles);
		}
	}

	void tag_files (const Settings& parDB, const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, OwnerSetInfo parSet) {
		pq::Connection conn(std::string(parDB.username), std::string(parDB.password), std::string(parDB.dbname), std::string(parDB.address), parDB.port);
		conn.connect();

		if (parSet.is_valid) {
			if (parRegexes.size() == 1) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"group_id\"=$2 AND \"path\" ~ $3;";
				conn.query(query, parTags, parSet.group_id, parRegexes.front());
			}
			else if (parRegexes.size() > 1) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"group_id\"=$2 AND \"path\" ~ ANY($3);";
				conn.query(query, parTags, parSet.group_id, parRegexes);
			}
			else if (parRegexes.size() == 0) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) WHERE \"group_id\"=$2 ORDER BY 1);";
				conn.query(query, parTags, parSet.group_id);
			}
		}
		else {
			if (parRegexes.size() == 1) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"path\" ~ $2;";
				conn.query(query, parTags, parRegexes.front());
			}
			else if (parRegexes.size() > 1) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"path\" ~ ANY($2);";
				conn.query(query, parTags, parRegexes);
			}
			else if (parRegexes.size() == 0) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1);";
				conn.query(query, parTags);
			}
		}
	}

	void delete_tags (const Settings& parDB, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags, OwnerSetInfo parSet) {
		pq::Connection conn(std::string(parDB.username), std::string(parDB.password), std::string(parDB.dbname), std::string(parDB.address), parDB.port);
		conn.connect();

		if (parTags.size() == 1) {
			if (parSet.is_valid) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY_REMOVE(tags, $1) WHERE \"id\" = ANY($2) AND \"group_id\" = $3;";
				conn.query(query, parTags.front(), parFiles, parSet.group_id);
			}
			else {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY_REMOVE(tags, $1) WHERE \"id\" = ANY($2);";
				conn.query(query, parTags.front(), parFiles);
			}
		}
		else {
			if (parSet.is_valid) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT UNNEST(\"tags\") EXCEPT SELECT UNNEST($1)) WHERE \"id\" = ANY($2) AND \"group_id\" = $3;";
				conn.query(query, parTags, parFiles, parSet.group_id);
			}
			else {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT UNNEST(\"tags\") EXCEPT SELECT UNNEST($1)) WHERE \"id\" = ANY($2);";
				conn.query(query, parTags, parFiles);
			}
		}
	}

	void delete_tags (const Settings& parDB, const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, OwnerSetInfo parSet) {
		pq::Connection conn(std::string(parDB.username), std::string(parDB.password), std::string(parDB.dbname), std::string(parDB.address), parDB.port);
		conn.connect();

		if (parTags.size() == 1) {
			if (parSet.is_valid) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY_REMOVE(tags, $1) WHERE  \"group_id\" = $3 AND \"path\" ~ ANY($3);";
				conn.query(query, parTags.front(), parSet.group_id, parRegexes);
			}
			else {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY_REMOVE(tags, $1) WHERE \"path\" ~ ANY($2);";
				conn.query(query, parTags.front(), parRegexes);
			}
		}
		else {
			if (parSet.is_valid) {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT UNNEST(\"tags\") EXCEPT SELECT UNNEST($1)) WHERE \"group_id\" = $2 AND \"path\" ~ ANY($3);";
				conn.query(query, parTags, parSet.group_id, parRegexes);
			}
			else {
				const std::string query = "UPDATE \"files\" SET \"tags\" = ARRAY(SELECT UNNEST(\"tags\") EXCEPT SELECT UNNEST($1)) WHERE \"path\" = ANY($2);";
				conn.query(query, parTags, parRegexes);
			}
		}
	}

	void delete_all_tags (const Settings& parDB, const std::vector<uint64_t>& parFiles, OwnerSetInfo parSet) {
		pq::Connection conn(std::string(parDB.username), std::string(parDB.password), std::string(parDB.dbname), std::string(parDB.address), parDB.port);
		conn.connect();

		if (parSet.is_valid) {
			const std::string query =
				"UPDATE \"files\" SET \"tags\" = '{}' WHERE \"id\"=ANY($1) AND \"group_id\"=$2;";
			conn.query(query, parFiles, parSet.group_id);
		}
		else {
			const std::string query =
				"UPDATE \"files\" SET \"tags\" = '{}' WHERE \"id\"=ANY($1);";
			conn.query(query, parFiles);
		}
	}

	void delete_all_tags (const Settings& parDB, const std::vector<std::string>& parRegexes, OwnerSetInfo parSet) {
		pq::Connection conn(std::string(parDB.username), std::string(parDB.password), std::string(parDB.dbname), std::string(parDB.address), parDB.port);
		conn.connect();

		if (parSet.is_valid) {
			const std::string query = "UPDATE \"files\" SET \"tags\" = '{}' WHERE \"group_id\"=$1 AND \"path\" ~ ANY($2);";
			conn.query(query, parSet.group_id, parRegexes);
		}
		else {
			const std::string query = "UPDATE \"files\" SET \"tags\" = '{}' WHERE \"path\" ~ ANY($2);";
			conn.query(query, parRegexes);
		}
	}
} //namespace dinbpostgres
