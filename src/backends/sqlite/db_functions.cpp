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

#include "db_functions.hpp"
#include "SQLiteCpp/SQLiteCpp.h"
#include "dindexer-machinery/recorddata.hpp"
#include "time_t_to_timestamp.hpp"
#include <ctime>

namespace dindb {
	namespace {
		template <typename T> T to (const SQLite::Column& parCol);
		template <> uint64_t to (const SQLite::Column& parCol) {
			static_assert(sizeof(long long int) == sizeof(uint64_t), "Unexpected type size");
			const auto v = static_cast<long long int>(parCol);
			return static_cast<uint64_t>(v);
		}
		template <> bool to (const SQLite::Column& parCol) {
			const int v = parCol;
			return static_cast<bool>(v);
		}
		template <> uint16_t to (const SQLite::Column& parCol) {
			const int v = parCol;
			return static_cast<uint16_t>(v);
		}
		template <> uint32_t to (const SQLite::Column& parCol) {
			static_assert(sizeof(int) == sizeof(uint32_t), "Unexpected type size");
			const int v = parCol;
			return static_cast<uint32_t>(v);
		}
		template <> std::string to (const SQLite::Column& parCol) {
			const char* v = parCol;
			return std::string(v);
		}
		template <> char to (const SQLite::Column& parCol) {
			const char* v = parCol;
			return *v;
		}
	} //unnamed namespace

	void tag_files (SQLite::Database& parDB, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		SQLite::Transaction trans(parDB);

		if (InvalidGroupID != parSet) {
			//SQLite::Statement query(parDB, "UPDATE files SET tags =
		}

	}

	void tag_files (SQLite::Database& parDB, const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet) {
		SQLite::Transaction trans(parDB);

		trans.commit();
	}

	void write_to_db (SQLite::Database& parDB, const std::vector<mchlib::FileRecordData>& parData, const mchlib::SetRecordDataFull& parSetData, const std::string& parSignature) {
		SQLite::Transaction trans(parDB);

		SQLite::Statement query(
			parDB,
			"INSERT INTO "
			"sets(desc,type,app_name,content_type,fs_uuid,disk_label) "
			"VALUES(?,?,?,?,?,?);"
		);
		query.bind(1, parSetData.name);
		query.bind(2, std::string(&parSetData.type, 1));
		query.bind(3, parSignature);
		query.bind(4, std::string(&parSetData.content_type, 1));
		query.bind(5, parSetData.fs_uuid);
		query.bind(6, parSetData.disk_label);

		query.exec();
		const auto new_group_id = parDB.getLastInsertRowid();
		assert(new_group_id > 0);

		for (std::size_t z = 0; z < parData.size(); ++z) {
			SQLite::Statement query(
				parDB,
				"INSERT INTO files(path,hash,level,group_id,is_directory,"
				"is_symlink,size,access_time,modify_time,is_hash_valid,"
				"unreadable,mimetype,charset) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?);"
			);
			const auto& itm = parData[z];
			query.bind(1, std::string(itm.path()));
			query.bind(2, tiger_to_string(itm.hash));
			query.bind(3, itm.level);
			query.bind(4, new_group_id);
			query.bind(5, itm.is_directory);
			query.bind(6, itm.is_symlink);
			query.bind(7, static_cast<long long int>(itm.size));
			query.bind(8, time_t_to_timestamp(itm.atime));
			query.bind(9, time_t_to_timestamp(itm.mtime));
			query.bind(10, itm.hash_valid);
			query.bind(11, itm.unreadable);
			query.bind(12, std::string(itm.mime_type()));
			query.bind(13, std::string(itm.mime_charset()));
			query.exec();
		}
		trans.commit();
	}

	bool read_from_db (
		SQLite::Database& parDB,
		mchlib::FileRecordData& parItem,
		mchlib::SetRecordDataFull& parSet,
		const mchlib::TigerHash& parHash
	) {
		uint32_t group_id;
		{
			SQLite::Statement query(
				parDB,
				"SELECT path,level,group_id,is_directory,is_symlink,size "
				"FROM files WHERE hash=?;"
			);
			query.bind(1, tiger_to_string(parHash, true));
			if (not query.executeStep())
				return false;
			parItem.abs_path = to<std::string>(query.getColumn("path"));
			parItem.hash = parHash;
			parItem.level = to<uint16_t>(query.getColumn("level"));
			parItem.size = to<uint64_t>(query.getColumn("size"));
			parItem.is_directory = to<bool>(query.getColumn("is_directory"));
			parItem.is_symlink = to<bool>(query.getColumn("is_symlink"));
			group_id = query.getColumn("group_id");

			if (parItem.abs_path.size() != 1 or parItem.abs_path != "/") {
				parItem.abs_path = std::string("/") + parItem.abs_path;
			}
			parItem.path_offset = 1;
		}

		{
			SQLite::Statement query(
				parDB,
				"SELECT desc,type,disk_number,fs_uuid,disk_label,content_type "
				"FROM sets WHERE id=?;"
			);
			query.bind(1, group_id);

			bool no_results = true;
			if (query.executeStep()) {
				no_results = false;

				parSet.type = to<char>(query.getColumn("type"));
				parSet.name = to<std::string>(query.getColumn("name"));
				parSet.disk_number = to<uint32_t>(query.getColumn("disk_number"));
				parSet.fs_uuid = to<std::string>(query.getColumn("fs_uuid"));
				parSet.disk_label = to<std::string>(query.getColumn("disk_label"));
				parSet.content_type = to<char>(query.getColumn("content_type"));
			}

			if (no_results) {
				std::ostringstream err_msg;
				err_msg << "Missing set: found a record with group_id=" << group_id;
				err_msg << " but there is no such id in table \"sets\"";
				throw std::length_error(err_msg.str());
			}
		}

		return true;
	}
} //namespace dindb
