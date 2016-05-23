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

#ifndef idB070B86E0E4047B1AF4144DEF2759F3C
#define idB070B86E0E4047B1AF4144DEF2759F3C

#include <functional>
#include <vector>
#include <string>
#include <cstdint>
#include <map>

namespace dinbpostgres {
	struct Settings;

	using IDDescMap = std::map<uint32_t, std::string>;
	using ConfirmDeleCallback = std::function<bool(const IDDescMap&)>;

	void delete_group_from_db ( const Settings& parDB, const std::vector<uint32_t>& parIDs, ConfirmDeleCallback parConf );
} //namespace dinbpostgres

#endif
