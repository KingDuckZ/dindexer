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

#include "backend_postgresql.hpp"
#include "backends/exposed_functions.hpp"
#include "pq/connection.hpp"
#include <ciso646>
#include <utility>
#include <cassert>
#include <yaml-cpp/yaml.h>

namespace YAML {
	template<>
	struct convert<dindb::Settings> {
		static Node encode (const dindb::Settings& parSettings) {
			Node node;
			node["address"] = parSettings.address;
			node["username"] = parSettings.username;
			node["password"] = parSettings.password;
			node["port"] = parSettings.port;
			node["dbname"] = parSettings.dbname;
			return node;
		}

		static bool decode (const Node& parNode, dindb::Settings& parSettings) {
			if (not parNode.IsMap() or parNode.size() != 5) {
				return false;
			}

			parSettings.address = parNode["address"].as<std::string>();
			parSettings.username = parNode["username"].as<std::string>();
			parSettings.password = parNode["password"].as<std::string>();
			parSettings.dbname = parNode["dbname"].as<std::string>();
			parSettings.port = parNode["port"].as<uint16_t>();
			return true;
		}
	};
} //namespace YAML

namespace dindb {
	BackendPostgreSql::BackendPostgreSql (std::string&& parUser, std::string&& parPass, std::string&& parDB, std::string&& parAddr, uint16_t parPort) :
		m_conn(new pq::Connection(std::move(parUser), std::move(parPass), std::move(parDB), std::move(parAddr), parPort))
	{
		assert(m_conn);
		m_conn->connect();
	}

	BackendPostgreSql::~BackendPostgreSql() noexcept {
		m_conn->disconnect();
	}

	void BackendPostgreSql::tag_files (const std::vector<FileIDType>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) const {
		if (InvalidGroupID != parSet) {
			const std::string query =
				"UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"id\"=ANY($2) AND \"group_id\"=$3;";
			m_conn->query(query, parTags, parFiles, parSet);
		}
		else {
			const std::string query =
				"UPDATE \"files\" SET \"tags\" = ARRAY(SELECT DISTINCT UNNEST(\"tags\" || $1) ORDER BY 1) WHERE \"id\"=ANY($2);";
			m_conn->query(query, parTags, parFiles);
		}
	}

	void BackendPostgreSql::tag_files (const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) const {
	}

	void BackendPostgreSql::delete_tags (const std::vector<FileIDType>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) const {
	}

	void BackendPostgreSql::delete_tags (const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) const {
	}

	void BackendPostgreSql::delete_all_tags (const std::vector<FileIDType>& parFiles, GroupIDType parSet) const {
	}

	void BackendPostgreSql::delete_all_tags (const std::vector<std::string>& parRegexes, GroupIDType parSet) const {
	}
} //namespace dindb

extern "C" dindb::Backend* create_backend (const YAML::Node*) {
	return new dindb::BackendPostgreSql("A", "B", "C", "D", 1);
}

extern "C" void destroy_backend (dindb::Backend* parDele) {
	if (parDele)
		delete parDele;
}
