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

#include "delete.hpp"
#include "pq/connection.hpp"
#include "duckhandy/infix_iterator.hpp"
#include <sstream>
#include <utility>
#include <iterator>
#include <ciso646>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/lexical_cast.hpp>
#include <cassert>

namespace dindb {
	namespace {
		IDDescMap fetch_existing_ids (pq::Connection& parConn, const std::vector<GroupIDType>& parIDs) {
			using boost::lexical_cast;

			IDDescMap retmap;
			if (parIDs.empty()) {
				return retmap;
			}

			std::ostringstream oss;
			oss << "SELECT \"id\",\"desc\" FROM \"sets\" WHERE \"id\"=";
			boost::copy(parIDs, infix_ostream_iterator<GroupIDType>(oss, " OR \"id\"="));
			oss << ';';

			auto resultset = parConn.query(oss.str());
			for (const auto& record : resultset) {
				retmap[lexical_cast<IDDescMap::key_type>(record["id"])] = record["desc"];
			}
			return retmap;
		}
	} //unnamed namespace

	void delete_group_from_db (pq::Connection& parDB, const std::vector<GroupIDType>& parIDs, ConfirmDeleCallback parConf) {
		assert(parDB.is_connected());

		const auto dele_ids = fetch_existing_ids(parDB, parIDs);
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
		boost::copy(dele_ids | boost::adaptors::map_keys, infix_ostream_iterator<GroupIDType>(oss, " OR \"group_id\"="));
		oss << ";\nDELETE FROM \"sets\" WHERE \"id\"=";
		boost::copy(dele_ids | boost::adaptors::map_keys, infix_ostream_iterator<GroupIDType>(oss, " OR \"id\"="));
		oss << ";\nCOMMIT;";

		parDB.query(oss.str());
	}
} //namespace dindb
