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

#include "tag_postgres.hpp"
#include "pq/connection.hpp"
#include "dindexer-common/settings.hpp"
#include <ciso646>

namespace din {
	void tag_files (const dinlib::SettingsDB& parDB, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags) {
		pq::Connection conn(std::string(parDB.username), std::string(parDB.password), std::string(parDB.dbname), std::string(parDB.address), parDB.port);
		conn.connect();

		const std::string query =
			"UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"id\"=ANY($2);";

		conn.query(query, parTags, parFiles);
	}

	void tag_files (const dinlib::SettingsDB& parDB, std::string parRegex, bool parCaseSensitive, const std::vector<boost::string_ref>& parTags) {
		pq::Connection conn(std::string(parDB.username), std::string(parDB.password), std::string(parDB.dbname), std::string(parDB.address), parDB.port);
		conn.connect();

		if (not parCaseSensitive)
			parRegex = "(?i)" + parRegex;

		const std::string query =
			"UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"path\" ~ $2;";

		conn.query(query, parTags, parRegex);
	}
} //namespace din
