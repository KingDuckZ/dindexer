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
#include <utility>
#include <sstream>
#include <boost/utility/string_ref.hpp>
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
			return std::move(conn);
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

			return std::move(retval);
		}
	} //unnamed namespace

	std::vector<LocatedItem> locate_in_db (const dinlib::SettingsDB& parDB, const std::string& parSearch, bool parCaseInsensitive) {
		using boost::lexical_cast;
		using boost::string_ref;
		namespace ba = boost::algorithm;

		auto conn = make_pq_conn(parDB);

		const auto clean_string_with_quotes = conn.escaped_literal(parSearch);
		const auto clean_string = string_ref(clean_string_with_quotes).substr(1, clean_string_with_quotes.size() - 2);

		std::ostringstream oss;
		oss << "SELECT \"path\",\"id\",\"group_id\" FROM \"files\" WHERE ";
		if (parCaseInsensitive) {
			std::string lower(clean_string);
			ba::to_lower(lower);
			oss << "LOWER(\"path\") LIKE '%" << lower << "%' ";
		}
		else {
			oss << "\"path\" LIKE '%" << clean_string << "%' ";
		}
		oss << "LIMIT " << g_max_results << ';';

		auto result = conn.query(oss.str());
		std::vector<LocatedItem> retval;
		retval.reserve(result.size());
		for (const auto& record : result) {
			retval.push_back(LocatedItem{
				record["path"],
				lexical_cast<decltype(LocatedItem::id)>(record["id"]),
				lexical_cast<decltype(LocatedItem::group_id)>(record["group_id"])
			});
		}

		return std::move(retval);
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
