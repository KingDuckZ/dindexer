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

#ifndef id48FE261DDC9F4F608C9E82FCCA4E55C0
#define id48FE261DDC9F4F608C9E82FCCA4E55C0

#include "backends/db_backend.hpp"
#include <vector>
#include <string>
#include <boost/utility/string_ref.hpp>

namespace SQLite {
	class Database;
} //namespace SQLite

namespace mchlib {
	struct FileRecordData;
	struct SetRecordDataFull;
	struct TigerHash;
} //namespace mchlib

namespace dindb {
	//Tag
	void tag_files (SQLite::Database& parDB, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet);
	void tag_files (SQLite::Database& parDB, const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet);

	//Scan
	void write_to_db (SQLite::Database& parDB, const std::vector<mchlib::FileRecordData>& parData, const mchlib::SetRecordDataFull& parSetData, const std::string& parSignature);
	bool read_from_db (SQLite::Database& parDB, mchlib::FileRecordData& parItem, mchlib::SetRecordDataFull& parSet, const mchlib::TigerHash& parHash);
} //namespace dindb

#endif
