/* Copyright 2016, Michele Santullo
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

#include "dindexer-machinery/machinery_info.hpp"
#include "dindexerConfig.h"
#include "helpers/stringize.h"

namespace mchlib {
	boost::string_ref lib_signature() {
		return boost::string_ref("machinery_v"
			STRINGIZE(VERSION_MAJOR) "."
			STRINGIZE(VERSION_MINOR) "."
			STRINGIZE(VERSION_PATCH)
#if VERSION_BETA
			"b"
#endif
		);
	}
} //namespace mchlib
