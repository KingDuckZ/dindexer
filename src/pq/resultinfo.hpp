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

#ifndef idB756FAED6EE34FB18E302528CEA7C1D7
#define idB756FAED6EE34FB18E302528CEA7C1D7

#include <libpq-fe.h>
#include <map>
#include <string>

namespace pq {
	struct ResultInfo {
		PGresult* result;
		const std::map<std::string, int>* column_mappings;
	};
} //namespace pq

#endif
