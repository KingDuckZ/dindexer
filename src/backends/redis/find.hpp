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

#ifndef idB4972996B4494E66A03B6AE205B1FA36
#define idB4972996B4494E66A03B6AE205B1FA36

#include "backends/db_backend.hpp"
#include "duckhandy/MaxSizedArray.hpp"
#include <vector>
#include <string>
#include <boost/utility/string_ref.hpp>
#include <cstdint>

namespace redis {
	class IncRedis;
} //namespace redis

namespace dindb {
	std::vector<GroupIDType> find_all_sets ( redis::IncRedis& parRedis );
	std::vector<LocatedItem> locate_in_db ( redis::IncRedis& parRedis, const std::string& parRegex, const TagList& parTags );
	std::vector<LocatedItem> locate_in_db ( redis::IncRedis& parRedis, const mchlib::TigerHash& parSearch, const TagList& parTags );
	std::vector<LocatedSet> locate_sets_in_db ( redis::IncRedis& parRedis, const std::string& parSubstr, bool parCaseInsensitive );
	std::vector<LocatedSet> locate_sets_in_db ( redis::IncRedis& parRedis, const std::string& parSubstr, const std::vector<GroupIDType>& parSets, bool parCaseInsensitive );
	std::vector<dhandy::MaxSizedArray<std::string, 4>> find_set_details ( redis::IncRedis& parRedis, const std::vector<GroupIDType>& parSets );
	std::vector<dhandy::MaxSizedArray<std::string, 1>> find_file_details ( redis::IncRedis& parRedis, GroupIDType parSetID, uint16_t parLevel, boost::string_ref parDir );
	std::vector<std::string> find_paths_starting_by ( redis::IncRedis& parRedis, GroupIDType parGroupID, uint16_t parLevel, boost::string_ref parPath );
} //namespace dindb

#endif
