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

#ifndef id842AF56BD80A4CF59957451DF9082AA2
#define id842AF56BD80A4CF59957451DF9082AA2

#include <string>
#include <vector>
#include <cstdint>
#include <boost/utility/string_ref.hpp>

namespace din {
	struct DinDBSettings;

	struct FileRecordData {
		const std::string path;
		const std::string hash;
		const uint16_t level;
		const uint64_t size;
		const bool is_directory;
		const bool is_symlink;
	};

	struct SetRecordData {
		const boost::string_ref name;
		const char type;
	};

	void write_to_db ( const DinDBSettings& parDB, const std::vector<FileRecordData>& parData, const SetRecordData& parSetData );
} //namespace din

#endif
