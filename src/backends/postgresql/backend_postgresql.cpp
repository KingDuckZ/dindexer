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
#include "tag.hpp"
#include "delete.hpp"
#include "pq/connection.hpp"
#include <ciso646>
#include <utility>
#include <cassert>
#include <yaml-cpp/yaml.h>

namespace dindb {
	namespace {
		struct PostgreConnectionSettings {
			std::string address;
			std::string username;
			std::string password;
			std::string dbname;
			uint16_t port;
		};
	} //unnamed namespace
} //namespace dindb

namespace YAML {
	template<>
	struct convert<dindb::PostgreConnectionSettings> {
		static Node encode (const dindb::PostgreConnectionSettings& parSettings) {
			Node node;
			node["address"] = parSettings.address;
			node["username"] = parSettings.username;
			node["password"] = parSettings.password;
			node["port"] = parSettings.port;
			node["dbname"] = parSettings.dbname;
			return node;
		}

		static bool decode (const Node& parNode, dindb::PostgreConnectionSettings& parSettings) {
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

	void BackendPostgreSql::tag_files (const std::vector<FileIDType>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		dindb::tag_files(*m_conn, parFiles, parTags, parSet);
	}

	void BackendPostgreSql::tag_files (const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		dindb::tag_files(*m_conn, parRegexes, parTags, parSet);
	}

	void BackendPostgreSql::delete_tags (const std::vector<FileIDType>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		dindb::delete_tags(*m_conn, parFiles, parTags, parSet);
	}

	void BackendPostgreSql::delete_tags (const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		dindb::delete_tags(*m_conn, parRegexes, parTags, parSet);
	}

	void BackendPostgreSql::delete_all_tags (const std::vector<FileIDType>& parFiles, GroupIDType parSet) {
		dindb::delete_all_tags(*m_conn, parFiles, parSet);
	}

	void BackendPostgreSql::delete_all_tags (const std::vector<std::string>& parRegexes, GroupIDType parSet) {
		dindb::delete_all_tags(*m_conn, parRegexes, parSet);
	}

	void BackendPostgreSql::delete_group (const std::vector<uint32_t>& parIDs, ConfirmDeleCallback parConf) {
		dindb::delete_group_from_db(*m_conn, parIDs, parConf);
	}
} //namespace dindb

extern "C" dindb::Backend* dindexer_create_backend (const YAML::Node* parConfig) {
	if (not parConfig)
		return nullptr;

	auto config = parConfig->as<dindb::PostgreConnectionSettings>();
	return new dindb::BackendPostgreSql(
		std::move(config.username),
		std::move(config.password),
		std::move(config.dbname),
		std::move(config.address),
		config.port
	);
}

extern "C" void dindexer_destroy_backend (dindb::Backend* parDele) {
	if (parDele)
		delete parDele;
}

extern "C" const char* dindexer_backend_name() {
	return "postgresql";
}
