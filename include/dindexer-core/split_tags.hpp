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

#ifndef id913CE9D2F60745349F39F2C82455973E
#define id913CE9D2F60745349F39F2C82455973E

#include <vector>
#include <boost/utility/string_ref.hpp>
#include "duckhandy/compatibility.h"

namespace dincore {
	std::vector<boost::string_ref> split_and_trim ( boost::string_ref parList, char parSeparator ) a_pure;
	std::vector<boost::string_ref> split_tags ( boost::string_ref parCommaSeparatedList ) a_pure;
	std::vector<boost::string_ref> split ( boost::string_ref parList, char parSeparator, bool parTrim, bool parDeleteEmpty ) a_pure;
} //namespace dincore

#endif
