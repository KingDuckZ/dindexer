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
#include "helpers/lexical_cast.hpp"
#include "dindexerConfig.h"
#include "helpers/stringize.h"
#include "tag.hpp"
#include "record_data_adapt.hpp"
#include <utility>
#include <yaml-cpp/yaml.h>
#include <array>
#include <cstdint>
#include <boost/range/empty.hpp>
#include <fstream>

namespace dindb {
	namespace {
		struct RedisConnectionSettings {
			std::string address;
			uint16_t port;
			uint16_t database;
		};

		std::string read_script (const dincore::SearchPaths& parSearch, const char* parName) {
			const auto full_path = parSearch.first_hit(boost::string_ref(parName));
			if (full_path.empty()) {
				const std::string msg = std::string("Unable to locate and load Lua script \"") + parName + "\" from any of the given search paths";
				throw std::runtime_error(msg);
			}

			std::ifstream script(full_path);
			std::string retval;
			script.seekg(0, std::ios::end);
			retval.reserve(script.tellg());
			script.seekg(0, std::ios::beg);
			retval.assign(std::istreambuf_iterator<char>(script), std::istreambuf_iterator<char>());

			return retval;
		}
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
			else
				parSettings.database = 0;
			return true;
		}
	};
} //namespace YAML

namespace dindb {
	BackendRedis::BackendRedis(std::string&& parAddress, uint16_t parPort, uint16_t parDatabase, bool parConnect, dincore::SearchPaths&& parLuaPaths) :
		m_redis(std::move(parAddress), parPort),
		m_tag_if_in_set(),
		m_lua_script_paths(std::move(parLuaPaths)),
		m_database(parDatabase)
	{
		if (parConnect)
			this->connect();
	}

	BackendRedis::~BackendRedis() noexcept {
	}

	void BackendRedis::connect() {
		using dinhelp::lexical_cast;

		m_redis.connect();
		m_redis.wait_for_connect();
		if (m_redis.is_connected()) {
			auto batch = m_redis.make_batch();
			batch.run("SELECT", lexical_cast<std::string>(m_database));
			batch.run("CLIENT", "SETNAME", PROGRAM_NAME "_v" STRINGIZE(VERSION_MAJOR) "." STRINGIZE(VERSION_MINOR) "." STRINGIZE(VERSION_PATCH));
			batch.throw_if_failed();
		}
		else {
			std::ostringstream oss;
			oss << "Error connecting to Redis: " << m_redis.connection_error();
			throw std::runtime_error(oss.str());
		}

		m_tag_if_in_set = m_redis.make_script(read_script(m_lua_script_paths, "tag_if_in_set.lua"));
		m_dele_tag_if_in_set = m_redis.make_script(read_script(m_lua_script_paths, "dele_tag_if_in_set.lua"));
	}

	void BackendRedis::disconnect() {
		m_redis.disconnect();
	}

	void BackendRedis::tag_files (const std::vector<FileIDType>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		dindb::tag_files(m_redis, m_tag_if_in_set, parFiles, parTags, parSet);
	}

	void BackendRedis::tag_files (const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		dindb::tag_files(m_redis, m_tag_if_in_set, parRegexes, parTags, parSet);
	}

	void BackendRedis::delete_tags (const std::vector<FileIDType>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		dindb::delete_tags(m_redis, m_dele_tag_if_in_set, parFiles, parTags, parSet);
	}

	void BackendRedis::delete_tags (const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		dindb::delete_tags(m_redis, m_dele_tag_if_in_set, parRegexes, parTags, parSet);
	}

	void BackendRedis::delete_all_tags (const std::vector<FileIDType>& parFiles, GroupIDType parSet) {
		dindb::delete_all_tags(m_redis, m_dele_tag_if_in_set, parFiles, parSet);
	}

	void BackendRedis::delete_all_tags (const std::vector<std::string>& parRegexes, GroupIDType parSet) {
		dindb::delete_all_tags(m_redis, m_dele_tag_if_in_set, parRegexes, parSet);
	}

	void BackendRedis::delete_group (const std::vector<GroupIDType>& parIDs, ConfirmDeleCallback parConf) {
	}

	void BackendRedis::write_files (const std::vector<mchlib::FileRecordData>& parData, const mchlib::SetRecordDataFull& parSetData, const std::string& parSignature) {
		using dinhelp::lexical_cast;
		using boost::string_ref;

		redis::Reply set_id_reply = m_redis.run("HINCRBY", PROGRAM_NAME ":indices", "set", "1");
		redis::Reply file_id_reply = m_redis.run("HINCRBY", PROGRAM_NAME ":indices", "files", lexical_cast<std::string>(parData.size()));

		const auto group_id = lexical_cast<std::string>(redis::get_integer(set_id_reply));
		const std::string set_key = PROGRAM_NAME ":set:" + group_id;
		const auto casted_data_size = static_cast<decltype(redis::get_integer(file_id_reply))>(parData.size());
		assert(redis::get_integer(file_id_reply) >= casted_data_size);
		const auto base_file_id = redis::get_integer(file_id_reply) - casted_data_size + 1;

		auto batch = m_redis.make_batch();

		batch.run(
			"HMSET",
			set_key,
			"name", parSetData.name,
			"disk_label", parSetData.disk_label,
			"fs_uuid", parSetData.fs_uuid,
			"type", parSetData.type,
			"content_type", parSetData.content_type
		);

		for (auto z = base_file_id; z < casted_data_size; ++z) {
			const std::string file_key = PROGRAM_NAME ":file:" + lexical_cast<std::string>(z);
			const auto& file_data = parData[z - base_file_id];
			const std::string hash = tiger_to_string(file_data.hash);
			batch.run(
				"HMSET",
				file_key,
				"hash", hash,
				"path", file_data.path(),
				"size", lexical_cast<std::string>(file_data.size),
				"level", lexical_cast<std::string>(file_data.level),
				"mime_type", file_data.mime_type(),
				"mime_charset", file_data.mime_charset(),
				"is_directory", (file_data.is_directory ? '1' : '0'),
				"is_symlink", (file_data.is_symlink ? '1' : '0'),
				"unreadable", (file_data.unreadable ? '1' : '0'),
				"hash_valid", (file_data.hash_valid ? '1' : '0'),
				"group_id", group_id
			);

			batch.run(
				"SADD",
				PROGRAM_NAME ":hash:" + hash,
				lexical_cast<std::string>(z)
			);
		}

		batch.throw_if_failed();
	}

	bool BackendRedis::search_file_by_hash (mchlib::FileRecordData& parItem, mchlib::SetRecordDataFull& parSet, const mchlib::TigerHash& parHash) {
		using boost::empty;

		const std::string hash_key = PROGRAM_NAME ":hash:" + tiger_to_string(parHash);
		redis::Reply hash_reply = m_redis.run("SRANDMEMBER", hash_key);
		if (redis::RedisVariantType_Nil == hash_reply.which() or (redis::RedisVariantType_Integer == hash_reply.which() and not redis::get_integer(hash_reply))) {
			return false;
		}
		else {
			const auto result_id = redis::get_string(hash_reply);
			auto set_key_and_file_item = redis::range_as<FileRecordDataWithGroup>(m_redis.hscan(result_id));
			parItem = std::move(set_key_and_file_item.second);
			const std::string group_key = std::move(set_key_and_file_item.first);

			auto scan_range = m_redis.hscan(group_key);
			if (empty(scan_range)) {
				return false;
			}
			else {
				parSet = redis::range_as<mchlib::SetRecordDataFull>(m_redis.hscan(group_key));
				return true;
			}
		}
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

	auto& config_node = *parConfig;
	auto config = config_node["connection"].as<dindb::RedisConnectionSettings>();

	auto vec = (config_node["script_paths"] ? config_node["script_paths"].as<std::vector<std::string>>() : std::vector<std::string>());
	dincore::SearchPaths lua_paths(std::move(vec));
	lua_paths.add_path(REDIS_SCRIPTS_PATH);

	return new dindb::BackendRedis(
		std::move(config.address),
		config.port,
		config.database,
		true,
		std::move(lua_paths)
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
