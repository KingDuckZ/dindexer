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

#include "backend_sqlite.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include "backends/exposed_functions.hpp"
#include "backends/backend_version.hpp"
#include "duckhandy/lexical_cast.hpp"
#include "dindexerConfig.h"
#include "duckhandy/stringize.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include <utility>
#include <yaml-cpp/yaml.h>
#include <array>
#include <cstdint>
#include <boost/range/empty.hpp>
#include <fstream>
#include <ctime>

namespace dindb {
	namespace {
		const char g_create_table_sets[] = R"(CREATE TABLE sets(
id INTEGER PRIMARY KEY AUTOINCREMENT,
desc TEXT NOT NULL,
type TEXT DEFAULT 'D' NOT NULL,
disk_number INTEGER DEFAULT 0 NOT NULL,
creation DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL,
app_name TEXT NOT NULL,
content_type TEXT DEFAULT 'G' NOT NULL,
disk_label TEXT NOT NULL,
fs_uuid TEXT NOT NULL
);)";

		const char g_create_table_files[] = R"(CREATE TABLE files(
id INTEGER PRIMARY KEY AUTOINCREMENT,
path TEXT NOT NULL,
level INTEGER NOT NULL,
group_id INTEGER NOT NULL,
is_directory INTEGER NOT NULL,
is_symlink INTEGER NOT NULL,
size INTEGER,
hash TEXT NOT NULL,
is_hash_valid INTEGER NOT NULL,
access_time DATETIME,
modify_time DATETIME,
unreadable INTEGER NOT NULL,
mimetype TEXT NOT NULL,
charset TEXT NOT NULL,
tags TEXT NOT NULL
);)";

		struct SqliteConnectionSettings {
			std::string db_path;
		};
	} //unnamed namespace
} //namespace dindb

namespace YAML {
	template<>
	struct convert<dindb::SqliteConnectionSettings> {
		static Node encode (const dindb::SqliteConnectionSettings& parSettings) {
			Node node;
			node["path"] = parSettings.db_path;
			return node;
		}

		static bool decode (const Node& parNode, dindb::SqliteConnectionSettings& parSettings) {
			if (not parNode.IsMap() or parNode.size() < 2) {
				return false;
			}

			parSettings.db_path = parNode["path"].as<std::string>();
			return true;
		}
	};
} //namespace YAML

namespace dindb {
	BackendSQLite::BackendSQLite (std::string&& parPath, bool parReadOnly, bool parConnectNow) :
		m_db_path(std::move(parPath)),
		m_db(),
		m_read_only(parReadOnly)
	{
		if (parConnectNow)
			this->connect();
	}

	BackendSQLite::~BackendSQLite() = default;

	void BackendSQLite::connect() {
		using SQLite::Database;
		using SQLite::OPEN_READONLY;
		using SQLite::OPEN_CREATE;

		assert(not m_db);
		if (not m_db)
			m_db.reset(new Database(m_db_path, (m_read_only ? OPEN_READONLY : OPEN_CREATE)));

		assert(m_db);
		if (not m_read_only) {
			if (not m_db->tableExists("files"))
				m_db->exec(g_create_table_files);
			if (not m_db->tableExists("sets"))
				m_db->exec(g_create_table_sets);
		}
	}

	void BackendSQLite::disconnect() {
		assert(m_db);
		m_db.reset();
	}

	void BackendSQLite::tag_files (const std::vector<FileIDType>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		//dindb::tag_files(m_redis, m_tag_if_in_set, parFiles, parTags, parSet);
	}

	void BackendSQLite::tag_files (const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		//dindb::tag_files(m_redis, m_tag_if_in_set, parRegexes, parTags, parSet);
	}

	void BackendSQLite::delete_tags (const std::vector<FileIDType>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		//dindb::delete_tags(m_redis, m_dele_tag_if_in_set, parFiles, parTags, parSet);
	}

	void BackendSQLite::delete_tags (const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		//dindb::delete_tags(m_redis, m_dele_tag_if_in_set, parRegexes, parTags, parSet);
	}

	void BackendSQLite::delete_all_tags (const std::vector<FileIDType>& parFiles, GroupIDType parSet) {
		//dindb::delete_all_tags(m_redis, m_dele_tag_if_in_set, parFiles, parSet);
	}

	void BackendSQLite::delete_all_tags (const std::vector<std::string>& parRegexes, GroupIDType parSet) {
		//dindb::delete_all_tags(m_redis, m_dele_tag_if_in_set, parRegexes, parSet);
	}

	void BackendSQLite::delete_group (const std::vector<GroupIDType>& parIDs, ConfirmDeleCallback parConf) {
		//delete_group_from_db(m_redis, m_dele_tag_if_in_set, m_dele_hash, parIDs, parConf);
	}

	void BackendSQLite::write_files (const std::vector<mchlib::FileRecordData>& parData, const mchlib::SetRecordDataFull& parSetData, const std::string& parSignature) {
		//using dhandy::lexical_cast;
		//using boost::string_ref;

		//const auto data_size = static_cast<int>(parData.size());
		//const auto group_id_int = m_redis.hincrby(PROGRAM_NAME ":indices", "set", 1);
		//const auto file_id_int = m_redis.hincrby(PROGRAM_NAME ":indices", "files", data_size);

		//const auto group_id = lexical_cast<std::string>(group_id_int);
		//const std::string set_key = PROGRAM_NAME ":set:" + group_id;
		//const std::string level_key = PROGRAM_NAME ":levels:" + group_id;
		//assert(file_id_int >= data_size);
		//const auto base_file_id = file_id_int - data_size + 1;

		//auto batch = m_redis.make_batch();

		//batch.hmset(
		//	set_key,
		//	"name", parSetData.name,
		//	"disk_label", parSetData.disk_label,
		//	"fs_uuid", parSetData.fs_uuid,
		//	"type", parSetData.type,
		//	"content_type", parSetData.content_type,
		//	"base_file_id", lexical_cast<std::string>(base_file_id),
		//	"item_count", lexical_cast<std::string>(parData.size()),
		//	"dir_count", lexical_cast<std::string>(std::count_if(parData.begin(), parData.end(), [](const mchlib::FileRecordData& r){return r.is_directory;})),
		//	"creation", lexical_cast<std::string>(std::time(nullptr)),
		//	"app_name", parSignature
		//);

//#if !defined(NDEBUG)
//		std::size_t inserted_count = 0;
//#endif
//		for (auto z = base_file_id; z < base_file_id + data_size; ++z) {
//			const std::string file_key = PROGRAM_NAME ":file:" + lexical_cast<std::string>(z);
//			assert(z >= base_file_id);
//			assert(static_cast<std::size_t>(z - base_file_id) < parData.size());
//			const auto& file_data = parData[z - base_file_id];
//			const std::string hash = tiger_to_string(file_data.hash);
//			batch.hmset(
//				file_key,
//				"hash", hash,
//				"path", file_data.path(),
//				"size", lexical_cast<std::string>(file_data.size),
//				"level", lexical_cast<std::string>(file_data.level),
//				"mime_type", file_data.mime_type(),
//				"mime_charset", file_data.mime_charset(),
//				"is_directory", (file_data.is_directory ? '1' : '0'),
//				"is_symlink", (file_data.is_symlink ? '1' : '0'),
//				"unreadable", (file_data.unreadable ? '1' : '0'),
//				"hash_valid", (file_data.hash_valid ? '1' : '0'),
//				"group_id", group_id,
//				"atime", lexical_cast<std::string>(file_data.atime),
//				"mtime", lexical_cast<std::string>(file_data.mtime)
//			);
//
//			batch.sadd(
//				PROGRAM_NAME ":hash:" + hash,
//				lexical_cast<std::string>(z)
//			);
//
//			batch.zadd(level_key, redis::IncRedisBatch::ZADD_None, false, static_cast<double>(file_data.level), file_key);
//#if !defined(NDEBUG)
//			++inserted_count;
//#endif
//		}
//		assert(inserted_count == parData.size());
//
//		batch.throw_if_failed();
	}

	bool BackendSQLite::search_file_by_hash (mchlib::FileRecordData& parItem, mchlib::SetRecordDataFull& parSet, const mchlib::TigerHash& parHash) {
//		using boost::empty;
//
//		const std::string hash_key = PROGRAM_NAME ":hash:" + tiger_to_string(parHash);
//		auto hash_reply = m_redis.srandmember(hash_key);
//		if (not hash_reply) {
//			return false;
//		}
//		else {
//			const auto file_key = PROGRAM_NAME ":file:" + *hash_reply;
//			auto set_key_and_file_item = redis::range_as<FileRecordDataWithGroup>(m_redis.hscan(file_key));
//			parItem = std::move(set_key_and_file_item.second);
//			assert(parItem.hash == parHash);
//			const std::string group_key = PROGRAM_NAME ":set:" + set_key_and_file_item.first;
//
//			auto scan_range = m_redis.hscan(group_key);
//			if (empty(scan_range)) {
//				return false;
//			}
//			else {
//				parSet = redis::range_as<mchlib::SetRecordDataFull>(m_redis.hscan(group_key));
//				return true;
//			}
//		}
	}

	std::vector<LocatedItem> BackendSQLite::locate_in_db (const std::string& parSearch, const TagList& parTags) {
		//return dindb::locate_in_db(m_redis, parSearch, parTags);
	}

	std::vector<LocatedItem> BackendSQLite::locate_in_db (const mchlib::TigerHash& parSearch, const TagList& parTags) {
		//return dindb::locate_in_db(m_redis, parSearch, parTags);
	}

	std::vector<LocatedSet> BackendSQLite::locate_sets_in_db (const std::string& parSubstr, bool parCaseInsensitive) {
		//return dindb::locate_sets_in_db(m_redis, parSubstr, parCaseInsensitive);
	}

	std::vector<LocatedSet> BackendSQLite::locate_sets_in_db (const std::string& parSubstr, const std::vector<GroupIDType>& parSets, bool parCaseInsensitive) {
		//return dindb::locate_sets_in_db(m_redis, parSubstr, parSets, parCaseInsensitive);
	}

	std::vector<GroupIDType> BackendSQLite::find_all_sets() {
		//return dindb::find_all_sets(m_redis);
	}

	std::vector<dhandy::MaxSizedArray<std::string, 4>> BackendSQLite::find_set_details (const std::vector<GroupIDType>& parSets) {
		//return dindb::find_set_details(m_redis, parSets);
	}

	std::vector<dhandy::MaxSizedArray<std::string, 1>> BackendSQLite::find_file_details (GroupIDType parSetID, uint16_t parLevel, boost::string_ref parDir) {
		//return dindb::find_file_details(m_redis, parSetID, parLevel, parDir);
	}

	std::vector<std::string> BackendSQLite::find_paths_starting_by (GroupIDType parGroupID, uint16_t parLevel, boost::string_ref parPath) {
		//return dindb::find_paths_starting_by(m_redis, parGroupID, parLevel, parPath);
	}
} //namespace dindb

extern "C" dindb::Backend* dindexer_create_backend (const YAML::Node* parConfig) {
	if (not parConfig)
		return nullptr;

	auto& config_node = *parConfig;
	auto config = config_node["connection"].as<dindb::SqliteConnectionSettings>();

	return new dindb::BackendSQLite(
		std::move(config.db_path),
		false,
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
