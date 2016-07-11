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

#include "find.hpp"
#include "command.hpp"
#include "helpers/lexical_cast.hpp"
#include "dindexerConfig.h"
#include "dindexer-core/split_tags.hpp"

namespace dindb {
	std::vector<GroupIDType> find_all_sets (redis::Command& parRedis) {
		using dincore::split_and_trim;
		using dinhelp::lexical_cast;

		std::vector<GroupIDType> retval;
		for (const auto& itm : parRedis.scan(PROGRAM_NAME ":set:*")) {
			retval.push_back(lexical_cast<GroupIDType>(split_and_trim(itm, ':').back()));
		}
		return retval;
	}
} //namespace dindb
