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

#ifndef idB2F92EE07A004D5293FD0657EEE8F75B
#define idB2F92EE07A004D5293FD0657EEE8F75B

#include "backends/db_backend.hpp"
#include "command.hpp"
#include <string>
#include <cstdint>

namespace dindb {
	class BackendRedis : public Backend {
	public:
		BackendRedis ( BackendRedis&& ) = default;
		BackendRedis ( std::string&& parAddress, uint16_t parPort, bool parConnect );
		virtual ~BackendRedis ( void ) noexcept;

		virtual void connect ( void ) override;
		virtual void disconnect ( void ) override;

		virtual void tag_files ( const std::vector<FileIDType>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet ) override;
		virtual void tag_files ( const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet ) override;
		virtual void delete_tags ( const std::vector<FileIDType>& parFiles, const std::vector<boost::string_ref>& parTags, GroupIDType parSet ) override;
		virtual void delete_tags ( const std::vector<std::string>& parRegexes, const std::vector<boost::string_ref>& parTags, GroupIDType parSet ) override;
		virtual void delete_all_tags ( const std::vector<FileIDType>& parFiles, GroupIDType parSet ) override;
		virtual void delete_all_tags ( const std::vector<std::string>& parRegexes, GroupIDType parSet ) override;

		virtual void delete_group ( const std::vector<uint32_t>& parIDs, ConfirmDeleCallback parConf ) override;

		virtual void write_files ( const std::vector<mchlib::FileRecordData>& parData, const mchlib::SetRecordDataFull& parSetData, const std::string& parSignature ) override;
		virtual bool search_file_by_hash ( mchlib::FileRecordData& parItem, mchlib::SetRecordDataFull& parSet, const mchlib::TigerHash& parHash ) override;

		virtual std::vector<LocatedItem> locate_in_db ( const std::string& parSearch, const TagList& parTags ) override;
		virtual std::vector<LocatedItem> locate_in_db ( const mchlib::TigerHash& parSearch, const TagList& parTags ) override;
		virtual std::vector<LocatedSet> locate_sets_in_db ( const std::string& parSearch, bool parCaseInsensitive ) override;
		virtual std::vector<LocatedSet> locate_sets_in_db ( const std::string& parSearch, const std::vector<GroupIDType>& parSets, bool parCaseInsensitive ) override;

		virtual std::vector<GroupIDType> find_all_sets ( void ) override;
		virtual std::vector<dinhelp::MaxSizedArray<std::string, 4>> find_set_details ( const std::vector<GroupIDType>& parSets ) override;
		virtual std::vector<dinhelp::MaxSizedArray<std::string, 1>> find_file_details ( GroupIDType parSetID, uint16_t parLevel, boost::string_ref parDir ) override;
		virtual std::vector<std::string> find_paths_starting_by ( GroupIDType parGroupID, uint16_t parLevel, boost::string_ref parPath ) override;

	private:
		redis::Command m_redis;
	};
} //namespace dindb

#endif
