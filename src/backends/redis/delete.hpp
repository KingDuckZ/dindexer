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

#ifndef id1E0F8FEB88EC4FED843FFEE7BAB624BB
#define id1E0F8FEB88EC4FED843FFEE7BAB624BB

#include "backends/db_backend.hpp"
#include <functional>
#include <vector>
#include <string>
#include <map>

namespace redis {
	class IncRedis;
	class Script;
} //namespace redis

namespace dindb {
	using IDDescMap = std::map<GroupIDType, std::string>;
	using ConfirmDeleCallback = std::function<bool(const IDDescMap&)>;

	void delete_group_from_db (
		redis::IncRedis& parRedis,
		redis::Script& parDeleTagIfInSet,
		redis::Script& parDeleHash,
		const std::vector<GroupIDType>& parIDs,
		ConfirmDeleCallback parConf
	);
} //namespace dindb

#endif
