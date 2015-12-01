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
#include "settings.hpp"
#include <string>
#include <sstream>
#include <utility>
#include <boost/lexical_cast.hpp>
#include <exception>
#include <memory>
#include <boost/utility/string_ref.hpp>

namespace din {
	namespace {
		const std::size_t g_batch_size = 100;

		std::string make_set_insert_query (pq::Connection& parConn, const SetRecordData& parSetData) {
			std::ostringstream oss;
			oss << "INSERT INTO \"sets\" (\"desc\",\"type\") VALUES ("
				<< parConn.escaped_literal(parSetData.name) << ','
				<< '\'' << parSetData.type << '\''
				<< ");";
			return oss.str();
		}

		boost::string_ref time_to_str (const std::time_t parTime, char* parBuff, std::size_t parLength) {
			const auto gtm = std::gmtime(&parTime);
			const auto len = std::strftime(parBuff, parLength, "%F %T%z", gtm);
			return boost::string_ref(parBuff, len);
		}
	} //unnamed namespace

	bool read_from_db (FileRecordData& parItem, SetRecordDataFull& parSet, const DinDBSettings& parDB, std::string&& parHash) {
		using boost::lexical_cast;

		pq::Connection conn(std::string(parDB.username), std::string(parDB.password), std::string(parDB.dbname), std::string(parDB.address), parDB.port);
		conn.connect();

		uint32_t group_id;
		{
			std::ostringstream oss;
			oss << "SELECT path,level,group_id,is_directory,is_symlink,size FROM files WHERE hash=" <<
				conn.escaped_literal(parHash) <<
				" LIMIT 1;";

			auto resultset = conn.query(oss.str());
			if (resultset.empty()) {
				return false;
			}

			auto row = resultset[0];
			parItem.path = row["path"];
			parItem.hash = std::move(parHash);
			parItem.level = lexical_cast<uint16_t>(row["level"]);
			parItem.size = lexical_cast<uint64_t>(row["size"]);
			parItem.is_directory = (row["is_directory"] == "t" ? true : false);
			parItem.is_symlink = (row["is_symlink"] == "t" ? true : false);
			group_id = lexical_cast<uint32_t>(row["group_id"]);
		}

		{
			std::ostringstream oss;
			oss << "SELECT \"desc\",\"type\",\"disk_number\" FROM sets WHERE \"id\"=" << group_id << ';';

			auto resultset = conn.query(oss.str());
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

	void write_to_db (const DinDBSettings& parDB, const std::vector<FileRecordData>& parData, const SetRecordData& parSetData) {
		if (parData.empty()) {
			return;
		}

		const std::size_t strtime_buff_size = 512;
		std::unique_ptr<char[]> strtime_buff(new char[strtime_buff_size]);

		pq::Connection conn(std::string(parDB.username), std::string(parDB.password), std::string(parDB.dbname), std::string(parDB.address), parDB.port);
		conn.connect();

		conn.query_void("BEGIN;");
		conn.query_void(make_set_insert_query(conn, parSetData));
		//TODO: use COPY instead of INSERT INTO
		for (std::size_t z = 0; z < parData.size(); z += g_batch_size) {
			std::ostringstream query;
			query << "INSERT INTO \"files\" " <<
				"(path, hash, level, group_id, is_directory, is_symlink, size, " <<
				"access_time, modify_time) VALUES "
			;

			const char* comma = "";
			for (auto i = z; i < std::min(z + g_batch_size, parData.size()); ++i) {
				const auto& itm = parData[i];
				query << comma;
				query << '(' << conn.escaped_literal(itm.path) << ",'" << itm.hash << "',"
					<< itm.level << ','
					<< "currval('\"sets_id_seq\"')" << ','
					<< (itm.is_directory ? "true" : "false") << ','
					<< (itm.is_symlink ? "true" : "false") << ',' << itm.size
					<< ',' << '\'' << time_to_str(itm.atime, strtime_buff.get(), strtime_buff_size) << '\''
					<< ',' << '\'' << time_to_str(itm.mtime, strtime_buff.get(), strtime_buff_size) << '\''
				<< ')';
				comma = ",";
			}
			query << ';';
			//query << "\nCOMMIT;";

			conn.query_void(query.str());
		}
		conn.query_void("COMMIT;");
	}
} //namespace din
