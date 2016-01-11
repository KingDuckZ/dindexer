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

#include "dbbackend.hpp"
#include "pq/connection.hpp"
#include "dindexer-common/settings.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include <string>
#include <sstream>
#include <utility>
#include <boost/lexical_cast.hpp>
#include <exception>
#include <memory>
#include <boost/utility/string_ref.hpp>
#include <chrono>

namespace din {
	namespace {
	} //unnamed namespace

	bool read_from_db (mchlib::FileRecordData& parItem, mchlib::SetRecordDataFull& parSet, const dinlib::SettingsDB& parDB, const mchlib::TigerHash& parHash) {
		using boost::lexical_cast;

		pq::Connection conn(std::string(parDB.username), std::string(parDB.password), std::string(parDB.dbname), std::string(parDB.address), parDB.port);
		conn.connect();

		uint32_t group_id;
		{
			auto resultset = conn.query(
				"SELECT path,level,group_id,is_directory,is_symlink,size FROM files WHERE hash=$1 LIMIT 1;",
				tiger_to_string(parHash, true)
			);
			if (resultset.empty()) {
				return false;
			}

			auto row = resultset[0];
			parItem.abs_path = row["path"];
			parItem.hash = parHash;
			parItem.level = lexical_cast<uint16_t>(row["level"]);
			parItem.size = lexical_cast<uint64_t>(row["size"]);
			parItem.is_directory = (row["is_directory"] == "t" ? true : false);
			parItem.is_symlink = (row["is_symlink"] == "t" ? true : false);
			group_id = lexical_cast<uint32_t>(row["group_id"]);

			if (parItem.abs_path.size() != 1 or parItem.abs_path != "/") {
				parItem.abs_path = std::string("/") + parItem.abs_path;
			}
			parItem.path = boost::string_ref(parItem.abs_path).substr(1);
		}

		{
			auto resultset = conn.query(
				"SELECT \"desc\",\"type\",\"disk_number\" FROM sets WHERE \"id\"=$1;",
				group_id
			);
			if (resultset.empty()) {
				std::ostringstream err_msg;
				err_msg << "Missing set: found a record with group_id=" << group_id;
				err_msg << " but there is no such id in table \"sets\"";
				throw std::length_error(err_msg.str());
			}
			auto row = resultset[0];
			parSet.type = lexical_cast<char>(row["type"]);
			parSet.name = row["desc"];
			parSet.disk_number = lexical_cast<uint32_t>(row["disk_number"]);
		}
		return true;
	}

	void write_to_db (const dinlib::SettingsDB& parDB, const std::vector<mchlib::FileRecordData>& parData, const mchlib::SetRecordData& parSetData) {
		using std::chrono::system_clock;
		using boost::lexical_cast;

		if (parData.empty()) {
			return;
		}

		pq::Connection conn(std::string(parDB.username), std::string(parDB.password), std::string(parDB.dbname), std::string(parDB.address), parDB.port);
		conn.connect();

		conn.query("BEGIN;");
		uint32_t new_group_id;
		{
			auto id_res = conn.query("INSERT INTO \"sets\" (\"desc\",\"type\") VALUES ($1, $2) RETURNING \"id\";",
				parSetData.name,
				std::string(1, parSetData.type)
			);
			assert(id_res.size() == 1);
			assert(id_res[0].size() == 1);
			//TODO: make it possible to get the id directly as a string
			new_group_id = lexical_cast<uint32_t>(id_res[0][0]);
		}

		//TODO: remove this empty_path part. This is a temporary fix needed to
		//work around a bug in libpqtypes for which empty paths are inserted
		//as null values in the db.
		const char* const empty_path = "/";
		const auto empty_path_string = boost::string_ref(empty_path);

		for (std::size_t z = 0; z < parData.size(); ++z) {
			const std::string query = "INSERT INTO \"files\" (path, hash, "
				"level, group_id, is_directory, is_symlink, size, "
				"access_time, modify_time, is_hash_valid, unreadable, "
				"mimetype, charset) VALUES "
				"($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13);";

			const auto& itm = parData[z];
			assert(itm.path.data());
			conn.query(query,
				(itm.path.empty() ? empty_path_string : itm.path),
				tiger_to_string(itm.hash),
				itm.level,
				new_group_id,
				itm.is_directory,
				itm.is_symlink,
				itm.size,
				system_clock::from_time_t(itm.atime),
				system_clock::from_time_t(itm.mtime),
				itm.hash_valid,
				itm.unreadable,
				itm.mime_type,
				itm.mime_charset
			);
		}
		conn.query("COMMIT;");
	}
} //namespace din
