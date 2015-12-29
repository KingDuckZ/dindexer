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

namespace dinlib {
	struct SettingsDB;;
} //namespace dinlib

namespace din {
	struct FileRecordData;
	struct SetRecordData;
	struct SetRecordDataFull;
	struct TigerHash;

	void write_to_db ( const dinlib::SettingsDB& parDB, const std::vector<FileRecordData>& parData, const SetRecordData& parSetData );
	bool read_from_db ( FileRecordData& parItem, SetRecordDataFull& parSet, const dinlib::SettingsDB& parDB, const TigerHash& parHash );
} //namespace din

#endif
