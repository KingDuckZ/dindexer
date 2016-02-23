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

#include "globbing.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include <fnmatch.h>
#include <ciso646>
#include <cassert>

namespace mchlib {
	namespace implem {
		bool glob_matches (const FileRecordData& parData, const char* parGlob) {
			assert(parData.path.data());
			//assert that the substring in path terminates at the same place
			//where the one in abs_path terminates (ie: it's null-terminated).
			assert(parData.path == std::string(parData.path.data()));

			//See https://github.com/FlibbleMr/neolib/blob/master/include/neolib/string_utils.hpp
			//for an alternative to fnmatch() (grep wildcard_match)
			const int match = fnmatch(parGlob, parData.path.data(), FNM_PATHNAME);
			return not match;
		}
	} //namespace implem
} //namespace mchlib
