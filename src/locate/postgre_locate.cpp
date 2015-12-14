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
	} //unnamed namespace

	std::vector<LocatedItem> locate_in_db (const dinlib::SettingsDB& parDB, const std::string& parSearch, bool parCaseInsensitive) {
		using boost::lexical_cast;
		using boost::string_ref;
		namespace ba = boost::algorithm;

		pq::Connection conn(std::string(parDB.username), std::string(parDB.password), std::string(parDB.dbname), std::string(parDB.address), parDB.port);
		conn.connect();

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
} //namespace din
