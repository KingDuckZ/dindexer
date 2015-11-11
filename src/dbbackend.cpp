/* Copyright 2015, Michele Santullo
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

#include "dbbackend.hpp"
#include "pq/connection.hpp"
#include <string>
#include <sstream>
#include <utility>

namespace din {
	namespace {
	} //unnamed namespace

	void write_to_db (const std::vector<FileRecordData>& parData) {
		if (parData.empty()) {
			return;
		}

		std::ostringstream query;
		query << "BEGIN;\n";
		query << "INSERT INTO \"Files\" (path, hash, level, group_id, is_directory, is_symlink, size) VALUES ";

		pq::Connection conn("michele", "password", "dindexer", "100.200.100.200", 5432);
		conn.connect();

		const char* comma = "";
		for (const auto& itm : parData) {
			query << comma;
			query << '(' << conn.escape_literal(itm.path) << ",'" << itm.hash << "',"
				<< itm.level << ','
				<< 10 << ',' << (itm.is_directory ? "true" : "false") << ','
				<< (itm.is_symlink ? "true" : "false") << ',' << itm.size << ')'
			;
			comma = ",";
		}
		query << ';';
		query << "\nCOMMIT;";

		conn.query_void(query.str());
	}
} //namespace din
