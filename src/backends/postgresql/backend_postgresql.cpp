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
#include "backends/backend_version.hpp"
#include "tag.hpp"
#include "delete.hpp"
#include "scan.hpp"
#include "locate.hpp"
#include "navigate.hpp"
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

	void BackendPostgreSql::connect() {
		m_conn->connect();
	}

	void BackendPostgreSql::disconnect() {
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

	void BackendPostgreSql::write_files (const std::vector<mchlib::FileRecordData>& parData, const mchlib::SetRecordDataFull& parSetData, const std::string& parSignature) {
		dindb::write_to_db(*m_conn, parData, parSetData, parSignature);
	}

	bool BackendPostgreSql::search_file_by_hash ( mchlib::FileRecordData& parItem, mchlib::SetRecordDataFull& parSet, const mchlib::TigerHash& parHash) {
		return dindb::read_from_db(parItem, parSet, *m_conn, parHash);
	}

	std::vector<LocatedItem> BackendPostgreSql::locate_in_db (const std::string& parSearch, const TagList& parTags) {
		return dindb::locate_in_db(*m_conn, parSearch, parTags);
	}

	std::vector<LocatedItem> BackendPostgreSql::locate_in_db (const mchlib::TigerHash& parSearch, const TagList& parTags) {
		return dindb::locate_in_db(*m_conn, parSearch, parTags);
	}

	std::vector<LocatedSet> BackendPostgreSql::locate_sets_in_db (const std::string& parSearch, bool parCaseInsensitive) {
		return dindb::locate_sets_in_db(*m_conn, parSearch, parCaseInsensitive);
	}

	std::vector<LocatedSet> BackendPostgreSql::locate_sets_in_db (const std::string& parSearch, const std::vector<GroupIDType>& parSets, bool parCaseInsensitive) {
		return dindb::locate_sets_in_db(*m_conn, parSearch, parSets, parCaseInsensitive);
	}

	std::vector<GroupIDType> BackendPostgreSql::find_all_sets() {
		return dindb::find_all_sets(*m_conn);
	}

	std::vector<dinhelp::MaxSizedArray<std::string, 4>> BackendPostgreSql::find_set_details (const std::vector<GroupIDType>& parSets) {
		return dindb::find_set_details<
			dindb::SetDetail_ID,
			dindb::SetDetail_Desc,
			dindb::SetDetail_CreeationDate,
			dindb::SetDetail_DiskLabel
		>(*m_conn, parSets);
	}

	std::vector<dinhelp::MaxSizedArray<std::string, 1>> BackendPostgreSql::find_file_details (GroupIDType parSetID, uint16_t parLevel, boost::string_ref parDir) {
		return dindb::find_file_details<dindb::FileDetail_Path>(*m_conn, parSetID, parLevel, parDir);
	}

	std::vector<std::string> BackendPostgreSql::find_paths_starting_by (GroupIDType parGroupID, uint16_t parLevel, boost::string_ref parPath) {
		return dindb::find_paths_starting_by(*m_conn, parGroupID, parLevel, parPath);
	}
} //namespace dindb

extern "C" dindb::Backend* dindexer_create_backend (const YAML::Node* parConfig) {
	if (not parConfig)
		return nullptr;

	auto& config_node = *parConfig;
	auto config = config_node["connection"].as<dindb::PostgreConnectionSettings>();
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

extern "C" int dindexer_backend_iface_version() {
	return dindb::g_current_iface_version;
}
