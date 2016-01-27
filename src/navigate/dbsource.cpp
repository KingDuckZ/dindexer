/* Copyright 2016, Michele Santullo
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

#include "dbsource.hpp"
#include "dindexer-common/settings.hpp"
#include "pq/connection.hpp"
#include "helpers/infix_iterator.hpp"
#include <ciso646>
#include <utility>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <boost/range/algorithm/copy.hpp>

namespace din {
	struct DBSource::LocalData {
		explicit LocalData ( const dinlib::SettingsDB& parDBSettings ) :
			conn(
				std::string(parDBSettings.username),
				std::string(parDBSettings.password),
				std::string(parDBSettings.dbname),
				std::string(parDBSettings.address),
				parDBSettings.port
			)
		{
		}

		pq::Connection conn;
	};

	DBSource::DBSource (const dinlib::SettingsDB& parDBSettings) :
		m_local_data(new LocalData(parDBSettings))
	{
		assert(not m_local_data->conn.is_connected());
	}

	DBSource::~DBSource() noexcept {
	}

	void DBSource::disconnect() {
		if (m_local_data->conn.is_connected()) {
			m_local_data->conn.disconnect();
		}
	}

	pq::Connection& DBSource::get_conn() {
		if (not m_local_data->conn.is_connected()) {
			m_local_data->conn.connect();
		}
		return m_local_data->conn;
	}

	std::vector<uint32_t> DBSource::sets() {
		using boost::lexical_cast;

		auto& conn = get_conn();
		const std::string query = "SELECT \"id\" FROM \"sets\";";
		auto res = conn.query(query);
		std::vector<uint32_t> retval;

		retval.reserve(res.size());
		for (const auto& row : res) {
			retval.push_back(lexical_cast<uint32_t>(row[0]));
		}
		return std::move(retval);
	}

	void DBSource::query_push_results (const std::vector<std::string>& parCols, boost::string_ref parTable, const std::vector<uint32_t>& parIDs, std::function<void(std::string&&)> parCallback) {
		std::ostringstream oss;
		oss << "SELECT \"";
		boost::copy(parCols, infix_ostream_iterator<std::string>(oss, "\", \""));
		oss << '"';
		oss << " FROM \"" << parTable << "\" WHERE \"id\" = ANY($1) ORDER BY \"desc\" ASC LIMIT 500;";

		auto& conn = get_conn();
		auto result = conn.query(oss.str(), parIDs);
		for (auto row : result) {
			for (auto val : row) {
				parCallback(std::move(val));
			}
		}
	}
} //namespace din
