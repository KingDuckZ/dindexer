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

#ifndef id1AE05A59AE0E4A4490040FD85D9AF665
#define id1AE05A59AE0E4A4490040FD85D9AF665

#include <vector>
#include <string>
#include <cstdint>
#include <boost/utility/string_ref.hpp>

namespace mchlib {
	struct TigerHash;
} //namespace mchlib

namespace dinbpostgres {
	struct Settings;

	struct LocatedItem {
		std::string path;
		uint64_t id;
		uint32_t group_id;
	};

	struct LocatedSet {
		std::string desc;
		uint32_t id;
		uint32_t files_count;
		uint32_t dir_count;
	};

	using TagList = std::vector<boost::string_ref>;

	std::vector<LocatedItem> locate_in_db ( const Settings& parDB, const std::string& parSearch, const TagList& parTags );
	std::vector<LocatedItem> locate_in_db ( const Settings& parDB, const mchlib::TigerHash& parSearch, const TagList& parTags );
	std::vector<LocatedSet> locate_sets_in_db ( const Settings& parDB, const std::string& parSearch, bool parCaseInsensitive );
	std::vector<LocatedSet> locate_sets_in_db ( const Settings& parDB, const std::string& parSearch, const std::vector<uint32_t>& parSets, bool parCaseInsensitive );
} //namespace dinbpostgres

#endif
