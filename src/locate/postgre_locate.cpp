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

#include "postgre_locate.hpp"
#include "pq/connection.hpp"
#include "dindexer-machinery/tiger.hpp"
#include <utility>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace din {
	namespace {
		const int g_max_results = 200;

		pq::Connection make_pq_conn ( const dinlib::SettingsDB& parDB, bool parOpen=true );

		pq::Connection make_pq_conn (const dinlib::SettingsDB& parDB, bool parOpen) {
			auto conn = pq::Connection(std::string(parDB.username), std::string(parDB.password), std::string(parDB.dbname), std::string(parDB.address), parDB.port);
			if (parOpen) {
				conn.connect();
			}
			return conn;
		}

		std::vector<LocatedSet> sets_result_to_vec (pq::ResultSet&& parResult) {
			using boost::lexical_cast;

			std::vector<LocatedSet> retval;
			retval.reserve(parResult.size());
			for (const auto& record : parResult) {
				retval.push_back(LocatedSet{
					record["desc"],
					lexical_cast<decltype(LocatedSet::id)>(record["id"]),
					lexical_cast<decltype(LocatedSet::files_count)>(record["file_count"]),
					lexical_cast<decltype(LocatedSet::dir_count)>(record["dir_count"])
				});
			}

			return retval;
		}

		std::vector<LocatedItem> file_result_to_vec (pq::ResultSet&& parResult) {
			using boost::lexical_cast;

			std::vector<LocatedItem> retval;
			retval.reserve(parResult.size());
			for (const auto& record : parResult) {
				retval.push_back(LocatedItem{
					record["path"],
					lexical_cast<decltype(LocatedItem::id)>(record["id"]),
					lexical_cast<decltype(LocatedItem::group_id)>(record["group_id"])
				});
			}

			return retval;
		}

		template <typename... P>
		std::vector<LocatedItem> locate_in_db (pq::Connection& parConn, const char* parBaseQuery, std::size_t parBaseQueryLength, const char* parTagsParamToken, const TagList& parTags, const P&... parParams) {
			using boost::lexical_cast;

			assert(parTagsParamToken or parTags.empty());
			auto query = std::string(parBaseQuery, parBaseQueryLength);
			if (not parTags.empty()) {
				query += " AND \"tags\" @> ";
				query += parTagsParamToken;
			}
			query += " LIMIT ";
			query += lexical_cast<std::string>(g_max_results);
			query += ";";

			if (parTags.empty()) {
				auto result = parConn.query(query, parParams...);
				return file_result_to_vec(std::move(result));
			}
			else {
				auto result = parConn.query(query, parParams..., parTags);
				return file_result_to_vec(std::move(result));
			}
		}
	} //unnamed namespace

	std::vector<LocatedItem> locate_in_db (const dinlib::SettingsDB& parDB, const std::string& parSearch, bool parCaseInsensitive, const TagList& parTags) {
		auto conn = make_pq_conn(parDB);

		const auto clean_string_with_quotes = conn.escaped_literal(parSearch);

		const std::string search_regex = (parCaseInsensitive ? "(?i)" : "") + parSearch;
		const char base_query[] = "SELECT \"path\",\"id\",\"group_id\" FROM \"files\" WHERE \"path\" ~ $1";
		return locate_in_db(conn, base_query, sizeof(base_query) - 1, "$2", parTags, search_regex);
	}

	std::vector<LocatedItem> locate_in_db (const dinlib::SettingsDB& parDB, const mchlib::TigerHash& parSearch, const TagList& parTags) {
		auto conn = make_pq_conn(parDB);
		const char base_query[] = "SELECT \"path\",\"id\",\"group_id\" FROM \"files\" WHERE \"hash\"=$1";
		return locate_in_db(conn, base_query, sizeof(base_query) - 1, "$2", parTags, mchlib::tiger_to_string(parSearch, true));
	}

	std::vector<LocatedSet> locate_sets_in_db (const dinlib::SettingsDB& parDB, const std::string& parSearch, bool parCaseInsensitive) {
		auto conn = make_pq_conn(parDB);

		const std::string query = std::string("SELECT \"id\", \"desc\", "
			"(SELECT COUNT(*) FROM \"files\" WHERE \"group_id\"=\"sets\".\"id\" AND NOT \"is_directory\") as \"file_count\", "
			"(SELECT COUNT(*) FROM \"files\" WHERE \"group_id\"=\"sets\".\"id\" AND \"is_directory\") as \"dir_count\" "
			"FROM \"sets\" WHERE str_match_partial(\"desc\", $1, $2) LIMIT "
		) + std::to_string(g_max_results) + ";";

		auto result = conn.query(query, parSearch, parCaseInsensitive);
		return sets_result_to_vec(std::move(result));
	}

	std::vector<LocatedSet> locate_sets_in_db (const dinlib::SettingsDB& parDB, const std::string& parSearch, const std::vector<uint32_t>& parSets, bool parCaseInsensitive) {
		if (parSets.empty()) {
			return locate_sets_in_db(parDB, parSearch, parCaseInsensitive);
		}

		auto conn = make_pq_conn(parDB);

		const std::string query = std::string("SELECT \"id\", \"desc\", "
			"(SELECT COUNT(*) FROM \"files\" WHERE \"group_id\"=\"sets\".\"id\" AND NOT \"is_directory\") as \"file_count\", "
			"(SELECT COUNT(*) FROM \"files\" WHERE \"group_id\"=\"sets\".\"id\" AND \"is_directory\") as \"dir_count\" "
			"FROM \"sets\" WHERE \"id\" = ANY($1) AND str_match_partial(\"desc\", $3, $2) LIMIT "
		) + std::to_string(g_max_results) + ";";

		auto result = conn.query(query, parSearch, parCaseInsensitive, parSets);
		return sets_result_to_vec(std::move(result));
	}
} //namespace din
