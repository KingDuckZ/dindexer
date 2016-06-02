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

#include "navigate.hpp"
#include "pq/connection.hpp"
#include "helpers/infix_iterator.hpp"
#include "query_count_limit.hpp"
#include <ciso646>
#include <utility>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <boost/range/algorithm/copy.hpp>
#include <cassert>

namespace dindb {
	namespace {
		const uint32_t g_files_query_limit = g_query_count_limit;

		std::ostream& operator<< (std::ostream& parOut, const std::vector<std::string>& parCols) {
			parOut << '"';
			boost::copy(parCols, infix_ostream_iterator<std::string>(parOut, "\", \""));
			parOut << '"';
			return parOut;
		}
	} //unnamed namespace

	namespace implem {
		const SetDetailsMap g_set_details_map {
			{SetDetail_Desc, "desc"},
			{SetDetail_Type, "type"},
			{SetDetail_CreeationDate, "creation"},
			{SetDetail_AppName, "app_name"},
			{SetDetail_ID, "id"}
		};
		const FileDetailsMap g_file_details_map {
			{FileDetail_ID, "id"},
			{FileDetail_Path, "path"},
			{FileDetail_Level, "level"},
			{FileDetail_GroupID, "group_id"},
			{FileDetail_IsDir, "is_directory"},
			{FileDetail_IsSymLink, "is_symlink"},
			{FileDetail_Size, "size"},
			{FileDetail_Hash, "hash"},
			{FileDetail_IsHashValid, "is_hash_valid"},
			{FileDetail_ATime, "access_time"},
			{FileDetail_MTime, "modify_time"},
			{FileDetail_Unreadable, "unreadable"},
			{FileDetail_MimeType, "mimetype"},
			{FileDetail_Charset, "charset"}
		};

		void query_no_conditions (pq::Connection& parDB, const ColumnList& parCols, boost::string_ref parTable, const std::vector<GroupIDType>& parIDs, std::function<void(std::string&&)> parCallback) {
			assert(parDB.is_connected());

			std::ostringstream oss;
			oss << "SELECT " << parCols << ' ' <<
				"FROM \"" << parTable << "\" " <<
				"WHERE \"id\"=ANY($1) " <<
				"ORDER BY \"desc\" ASC " <<
				"LIMIT " << g_files_query_limit << ';';

			auto result = parDB.query(oss.str(), parIDs);
			for (auto row : result) {
				for (auto val : row) {
					parCallback(std::move(val));
				}
			}
		}

		void query_files_in_dir (pq::Connection& parDB, const ColumnList& parCols, boost::string_ref parDir, uint16_t parLevel, GroupIDType parGroupID, QueryCallback parCallback) {
			assert(parDB.is_connected());

			std::ostringstream oss;
			oss << "SELECT " << parCols << ' ' <<
				"FROM \"files\" WHERE " <<
				"\"level\"=$1 " <<
				"AND \"group_id\"=$2 " <<
				"AND str_begins_with(\"path\", COALESCE($3, '')) " <<
				"ORDER BY \"is_directory\" DESC, \"path\" ASC " <<
				"LIMIT " << g_files_query_limit << ';';

			auto result = parDB.query(
				oss.str(),
				parLevel,
				parGroupID,
				parDir
			);
			for (auto row : result) {
				for (auto val : row) {
					parCallback(std::move(val));
				}
			}
		}
	} //namespace implem

	std::vector<GroupIDType> find_all_sets (pq::Connection& parDB) {
		using boost::lexical_cast;

		assert(parDB.is_connected());

		const std::string query = "SELECT \"id\" FROM \"sets\";";
		auto res = parDB.query(query);
		std::vector<GroupIDType> retval;

		retval.reserve(res.size());
		for (const auto& row : res) {
			retval.push_back(lexical_cast<GroupIDType>(row[0]));
		}
		return retval;
	}

	std::vector<std::string> find_paths_starting_by (pq::Connection& parDB, GroupIDType parGroupID, uint16_t parLevel, boost::string_ref parPath) {
		assert(parDB.is_connected());

		std::ostringstream oss;
		oss << "SELECT \"path\" ||\n" <<
			"(SELECT CASE \"is_directory\"\n" <<
			"WHEN TRUE THEN '/'\n" <<
			"ELSE ''\n" <<
			"END) as path FROM \"files\" WHERE \"group_id\"=$1 AND " <<
			"\"level\"=$2 AND str_begins_with(\"path\", COALESCE($3, '')) " <<
			"ORDER BY \"is_directory\" DESC, \"path\" ASC LIMIT " <<
			g_files_query_limit << ';';

		auto result = parDB.query(
			oss.str(),
			parGroupID,
			parLevel,
			parPath
		);
		std::vector<std::string> retval;
		retval.reserve(retval.size());
		for (auto row : result) {
			assert(not row.empty());
			retval.push_back(row[0]);
		}
		return retval;
	}
} //namespace dindb
