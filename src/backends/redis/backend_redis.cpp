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
#include <utility>
#include <hiredis/hiredis.h>
#include <ciso646>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <yaml-cpp/yaml.h>

namespace dindb {
	namespace {
        using RedisReply = std::unique_ptr<redisReply, void(*)(void*)>;

		struct RedisConnectionSettings {
			std::string address;
			uint16_t port;
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
			return node;
		}

		static bool decode (const Node& parNode, dindb::RedisConnectionSettings& parSettings) {
			if (not parNode.IsMap() or parNode.size() != 2) {
				return false;
			}

			parSettings.address = parNode["address"].as<std::string>();
			parSettings.port = parNode["port"].as<uint16_t>();
			return true;
		}
	};
} //namespace YAML

namespace dindb {
    BackendRedis::BackendRedis(std::string &&parAddress, uint16_t parPort,
                               bool parConnect) :
            m_conn(nullptr, &redisFree),
            m_address(std::move(parAddress)),
            m_port(parPort) {
        if (parConnect)
            this->connect();
    }

    BackendRedis::~BackendRedis() noexcept {
    }

    void BackendRedis::connect() {
        if (not m_conn) {
			struct timeval timeout = {5, 500000}; //5.5 seconds?
			RedisConnection conn(
				redisConnectWithTimeout(m_address.c_str(), m_port, timeout),
				&redisFree
			);
			if (not conn) {
				std::ostringstream oss;
				oss << "Unable to connect to Redis server at " << m_address << ':' << m_port;
				throw std::runtime_error(oss.str());
			}
			if (conn->err) {
				std::ostringstream oss;
				oss << "Unable to connect to Redis server at " << m_address << ':' << m_port <<
					": " << conn->errstr;
				throw std::runtime_error(oss.str());
			}
			std::swap(conn, m_conn);
		}
    }

    void BackendRedis::disconnect() {
        m_conn.reset();
    }

    void BackendRedis::tag_files (const std::vector<FileIDType>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
    }

    void BackendRedis::tag_files (const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
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
		assert(is_connected());
		std::string key;
		{
			std::ostringstream key_oss;
			RedisReply incr_reply(static_cast<redisReply*>(redisCommand(m_conn.get(), "incr set_counter")), &freeReplyObject);
			key_oss << "sets:" << incr_reply->integer;
			key = key_oss.str();
		}

		RedisReply insert_reply(
			static_cast<redisReply*>(redisCommand(
				m_conn.get(),
				"hmset %b name %b disk_label %b fs_uuid %b",
				key.data(),
				key.size(),
				parSetData.name.data(),
				parSetData.name.size(),
				parSetData.disk_label.data(),
				parSetData.disk_label.size(),
				parSetData.fs_uuid.data(),
				parSetData.fs_uuid.size()
			)),
			&freeReplyObject
		);
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

	bool BackendRedis::is_connected() const {
		return m_conn and not m_conn->err;
	}
} //namespace dindb

extern "C" dindb::Backend* dindexer_create_backend (const YAML::Node* parConfig) {
	if (not parConfig)
		return nullptr;

	auto config = parConfig->as<dindb::RedisConnectionSettings>();
	return new dindb::BackendRedis(
		std::move(config.address),
		config.port,
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
