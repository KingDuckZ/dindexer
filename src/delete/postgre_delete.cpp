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

#include "postgre_delete.hpp"
#include "pq/connection.hpp"
#include "dindexer-common/settings.hpp"
#include "helpers/infix_iterator.hpp"
#include <sstream>
#include <utility>
#include <iterator>
#include <ciso646>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/lexical_cast.hpp>

namespace din {
	namespace {
		IDDescMap fetch_existing_ids (pq::Connection& parConn, const std::vector<uint32_t>& parIDs) {
			using boost::lexical_cast;

			IDDescMap retmap;
			if (parIDs.empty()) {
				return std::move(retmap);
			}

			std::ostringstream oss;
			oss << "SELECT \"id\",\"desc\" FROM \"sets\" WHERE \"id\"=";
			boost::copy(parIDs, infix_ostream_iterator<uint32_t>(oss, " OR \"id\"="));
			oss << ';';

			auto resultset = parConn.query(oss.str());
			for (const auto& record : resultset) {
				retmap[lexical_cast<IDDescMap::key_type>(record["id"])] = record["desc"];
			}
			return std::move(retmap);
		}
	} //unnamed namespace

	void delete_group_from_db (const dinlib::SettingsDB& parDB, const std::vector<uint32_t>& parIDs, ConfirmDeleCallback parConf) {
		pq::Connection conn(std::string(parDB.username), std::string(parDB.password), std::string(parDB.dbname), std::string(parDB.address), parDB.port);
		conn.connect();
		const auto dele_ids = fetch_existing_ids(conn, parIDs);
		if (dele_ids.empty()) {
			return;
		}
		if (not parConf(dele_ids)) {
			return;
		}

		std::vector<std::string> ids;
		ids.reserve(dele_ids.size());
		std::ostringstream oss;
		oss << "BEGIN;\nDELETE FROM \"files\" WHERE \"group_id\"=";
		boost::copy(dele_ids | boost::adaptors::map_keys, infix_ostream_iterator<uint32_t>(oss, " OR \"group_id\"="));
		oss << ";\nDELETE FROM \"sets\" WHERE \"id\"=";
		boost::copy(dele_ids | boost::adaptors::map_keys, infix_ostream_iterator<uint32_t>(oss, " OR \"id\"="));
		oss << ";\nCOMMIT;";

		conn.query(oss.str());
	}
} //namespace din
