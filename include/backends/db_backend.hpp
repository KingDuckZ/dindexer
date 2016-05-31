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

#ifndef id7506CA9825454B80856154ACFE8A9DE2
#define id7506CA9825454B80856154ACFE8A9DE2

#include "backends/backend_loader.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <boost/utility/string_ref.hpp>
#include <map>
#include <functional>

namespace dindb {
	using GroupIDType = uint32_t;
	using FileIDType = uint64_t;
	using IDDescMap = std::map<uint32_t, std::string>;
	using ConfirmDeleCallback = std::function<bool(const IDDescMap&)>;

	constexpr const GroupIDType InvalidGroupID = 0;
	constexpr const FileIDType InvalidFileID = 0;

	class Backend {
	public:
		Backend ( void ) = default;
		virtual ~Backend ( void ) noexcept = default;

		virtual void tag_files ( const std::vector<FileIDType>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet ) = 0;
		virtual void tag_files ( const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet ) = 0;
		virtual void delete_tags ( const std::vector<FileIDType>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet ) = 0;
		virtual void delete_tags ( const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet ) = 0;
		virtual void delete_all_tags ( const std::vector<FileIDType>& parFiles, GroupIDType parSet ) = 0;
		virtual void delete_all_tags ( const std::vector<std::string>& parRegexes, GroupIDType parSet ) = 0;

		virtual void delete_group ( const std::vector<uint32_t>& parIDs, ConfirmDeleCallback parConf ) = 0;

		virtual void write_files ( const std::vector<mchlib::FileRecordData>& parData, const mchlib::SetRecordData& parSetData, const std::string& parSignature ) = 0;
		virtual bool search_file_by_hash ( mchlib::FileRecordData& parItem, mchlib::SetRecordDataFull& parSet, const mchlib::TigerHash& parHash ) = 0;
	};
} //namespace dindb

#endif
