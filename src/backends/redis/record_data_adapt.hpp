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

#ifndef id2949D72CC2F246D4A289FFB820CC3A8F
#define id2949D72CC2F246D4A289FFB820CC3A8F

#include "dindexer-machinery/recorddata.hpp"
#include "helpers/lexical_cast.hpp"
#include <array>
#include <string>
#include <cstdint>
#include <utility>

namespace dindb {
	using FileRecordDataWithGroup = std::pair<std::string, mchlib::FileRecordData>;
} //namespace dindb

namespace redis {
	template <>
	struct StructAdapt<dindb::FileRecordDataWithGroup> {
		template <typename R>
		static bool decode (const R& parRange, dindb::FileRecordDataWithGroup& parOut) {
			using dinhelp::lexical_cast;

			std::array<std::string, 2> mime;
			std::string group_key;

			for (const auto itm : parRange) {
				if (itm.first == "path")
					parOut.second.abs_path = itm.second;
				else if (itm.first == "hash")
					parOut.second.hash = mchlib::string_to_tiger(itm.second);
				else if (itm.first == "size")
					parOut.second.size = lexical_cast<decltype(parOut.second.size)>(itm.second);
				else if (itm.first == "level")
					parOut.second.level = lexical_cast<decltype(parOut.second.level)>(itm.second);
				else if (itm.first == "mime_type")
					mime[0] = itm.second;
				else if (itm.first == "mime_charset")
					mime[1] = itm.second;
				else if (itm.first == "is_directory")
					parOut.second.is_directory = (itm.second[0] == '0' ? false : true);
				else if (itm.first == "is_symlink")
					parOut.second.is_symlink = (itm.second[0] == '0' ? false : true);
				else if (itm.first == "unreadable")
					parOut.second.unreadable = (itm.second[0] == '0' ? false : true);
				else if (itm.first == "hash_valid")
					parOut.second.hash_valid = (itm.second[0] == '0' ? false : true);
				else if (itm.first == "group_id")
					parOut.first = itm.second;
			}
			parOut.second.mime_full = mime[0] + mime[1];
			parOut.second.mime_type_offset = 0;
			parOut.second.mime_type_length = parOut.second.mime_charset_offset = static_cast<uint16_t>(mime[0].size());
			parOut.second.mime_charset_length = static_cast<uint16_t>(mime[1].size());
			return true;
		}
	};

	template <>
	struct StructAdapt<mchlib::SetRecordDataFull> {
		template <typename R>
		static bool decode (const R& parRange, mchlib::SetRecordDataFull& parOut) {
			using dinhelp::lexical_cast;

			for (const auto& itm : parRange) {
				if (itm.first == "name")
					parOut.name = itm.second;
				else if (itm.first == "disk_label")
					parOut.disk_label = itm.second;
				else if (itm.first == "fs_uuid")
					parOut.fs_uuid = itm.second;
				else if (itm.first == "type")
					parOut.type = itm.second[0];
				else if (itm.first == "content_type")
					parOut.content_type = itm.second[0];
			}
			return true;
		}
	};
} //namespace redis

#endif
