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

#ifndef id8E124FF76DF449CDB8FBA806F8EF4E78
#define id8E124FF76DF449CDB8FBA806F8EF4E78

#include "redisConfig.h"
#include <boost/utility/string_ref.hpp>
#if defined(WITH_CRYPTOPP)
#	include <set>
#else
#	include <map>
#endif
#include <string>
#include <array>
#include <boost/utility/string_ref.hpp>

namespace redis {
	class Command;

	class ScriptManager {
	public:
		explicit ScriptManager ( Command* parCommand );

		boost::string_ref submit_lua_script ( const std::string& parScript );

	private:
		using Sha1Array = std::array<char, 40>;

		boost::string_ref add_lua_script_ifn ( const std::string& parScript );

		Command* m_command;
#if defined(WITH_CRYPTOPP)
		std::set<Sha1Array> m_known_hashes;
#else
		std::map<std::string, Sha1Array> m_known_scripts;
#endif
	};

	inline boost::string_ref ScriptManager::submit_lua_script (const std::string& parScript) {
		return add_lua_script_ifn(parScript);
	}
} //namespace redis

#endif
