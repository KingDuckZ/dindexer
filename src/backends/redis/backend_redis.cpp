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

#include "backend_redis.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include "backends/exposed_functions.hpp"
#include "backends/backend_version.hpp"
#include "dindexerConfig.h"
#include <utility>
#include <yaml-cpp/yaml.h>
#include <boost/lexical_cast.hpp>

namespace dindb {
	namespace {
		struct RedisConnectionSettings {
			std::string address;
			uint16_t port;
			uint16_t database;
		};
	} //unnamed namespace
} //namespace dindb

namespace YAML {
	template<>
	struct convert<dindb::RedisConnectionSettings> {
		static Node encode (const dindb::RedisConnectionSettings& parSettings) {
			Node node;
			node["address"] = parSettings.address;
			node["port"] = parSettings.port;
			node["database"] = parSettings.database;
			return node;
		}

		static bool decode (const Node& parNode, dindb::RedisConnectionSettings& parSettings) {
			if (not parNode.IsMap() or parNode.size() != 2) {
				return false;
			}

			parSettings.address = parNode["address"].as<std::string>();
			parSettings.port = parNode["port"].as<uint16_t>();
			if (parNode["database"])
				parSettings.database = parNode["database"].as<uint16_t>();
			return true;
		}
	};
} //namespace YAML

namespace dindb {
	BackendRedis::BackendRedis(std::string &&parAddress, uint16_t parPort, uint16_t parDatabase, bool parConnect) :
		m_redis(std::move(parAddress), parPort),
		m_database(parDatabase)
	{
		if (parConnect)
			this->connect();
	}

	BackendRedis::~BackendRedis() noexcept {
	}

	void BackendRedis::connect() {
		using boost::lexical_cast;

		m_redis.connect();
		if (m_redis.is_connected() and m_database > 0) {
			const std::string command = "SELECT " + lexical_cast<std::string>(m_database);
			m_redis.run(command.c_str());
		}
	}

	void BackendRedis::disconnect() {
		m_redis.disconnect();
	}

	void BackendRedis::tag_files (const std::vector<FileIDType>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
	}

	void BackendRedis::tag_files (const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		for (const auto& file_path : m_redis.scan()) {
			std::cout << file_path << '\n';
		}
	}

	void BackendRedis::delete_tags (const std::vector<FileIDType>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
	}

	void BackendRedis::delete_tags (const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
	}

	void BackendRedis::delete_all_tags (const std::vector<FileIDType>& parFiles, GroupIDType parSet) {
	}

	void BackendRedis::delete_all_tags (const std::vector<std::string>& parRegexes, GroupIDType parSet) {
	}

	void BackendRedis::delete_group (const std::vector<uint32_t>& parIDs, ConfirmDeleCallback parConf) {
	}

	void BackendRedis::write_files (const std::vector<mchlib::FileRecordData>& parData, const mchlib::SetRecordDataFull& parSetData, const std::string& parSignature) {
		using boost::lexical_cast;

		auto incr_reply = m_redis.run("HINCRBY " PROGRAM_NAME ":indices set 1");
		const std::string set_key = PROGRAM_NAME ":set:" + lexical_cast<std::string>(redis::get_integer(incr_reply));
		auto insert_set_reply = m_redis.run("HMSET %b name %b disk_label %b fs_uuid %b", set_key, parSetData.name, parSetData.disk_label, parSetData.fs_uuid);
	}

	bool BackendRedis::search_file_by_hash (mchlib::FileRecordData& parItem, mchlib::SetRecordDataFull& parSet, const mchlib::TigerHash& parHash) {
		return false;
	}

	std::vector<LocatedItem> BackendRedis::locate_in_db (const std::string& parSearch, const TagList& parTags) {
		return std::vector<LocatedItem>();
	}

	std::vector<LocatedItem> BackendRedis::locate_in_db (const mchlib::TigerHash& parSearch, const TagList& parTags) {
		return std::vector<LocatedItem>();
	}

	std::vector<LocatedSet> BackendRedis::locate_sets_in_db (const std::string& parSearch, bool parCaseInsensitive) {
		return std::vector<LocatedSet>();
	}

	std::vector<LocatedSet> BackendRedis::locate_sets_in_db (const std::string& parSearch, const std::vector<GroupIDType>& parSets, bool parCaseInsensitive) {
		return std::vector<LocatedSet>();
	}

	std::vector<GroupIDType> BackendRedis::find_all_sets() {
		return std::vector<GroupIDType>();
	}

	std::vector<dinhelp::MaxSizedArray<std::string, 4>> BackendRedis::find_set_details (const std::vector<GroupIDType>& parSets) {
		return std::vector<dinhelp::MaxSizedArray<std::string, 4>>();
	}

	std::vector<dinhelp::MaxSizedArray<std::string, 1>> BackendRedis::find_file_details (GroupIDType parSetID, uint16_t parLevel, boost::string_ref parDir) {
		return std::vector<dinhelp::MaxSizedArray<std::string, 1>>();
	}

	std::vector<std::string> BackendRedis::find_paths_starting_by (GroupIDType parGroupID, uint16_t parLevel, boost::string_ref parPath) {
		return std::vector<std::string>();
	}
} //namespace dindb

extern "C" dindb::Backend* dindexer_create_backend (const YAML::Node* parConfig) {
	if (not parConfig)
		return nullptr;

	auto config = parConfig->as<dindb::RedisConnectionSettings>();
	return new dindb::BackendRedis(
		std::move(config.address),
		config.port,
		config.database,
		true
	);
}

extern "C" void dindexer_destroy_backend (dindb::Backend* parDele) {
	if (parDele)
		delete parDele;
}

extern "C" const char* dindexer_backend_name() {
	return "redis";
}

extern "C" int dindexer_backend_iface_version() {
	return dindb::g_current_iface_version;
}
