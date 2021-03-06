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

#ifndef idABF7D6D669F445B2852C4DDFC29C701E
#define idABF7D6D669F445B2852C4DDFC29C701E

#include "dindexer-core/split.hpp"

namespace dincore {
	std::vector<boost::string_ref> split_tags ( boost::string_ref parCommaSeparatedList ) a_pure;
	std::vector<boost::string_ref> split_and_trim ( boost::string_ref parCommaSeparatedList, char parSeparator ) a_pure;

	inline std::vector<boost::string_ref> split_tags (boost::string_ref parCommaSeparatedList) {
		return split(parCommaSeparatedList, ',', true, true);
	}

	inline std::vector<boost::string_ref> split_and_trim (boost::string_ref parCommaSeparatedList, char parSeparator) {
		return split(parCommaSeparatedList, parSeparator, true, true);
	}
} //namespace dincore

#endif
