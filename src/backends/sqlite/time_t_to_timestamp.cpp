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

#include "time_t_to_timestamp.hpp"
//#include <cassert>

namespace dindb {
	long long int time_t_to_timestamp (const time_t& parTime) {
		std::tm epoch_start;
		epoch_start.tm_sec = 0;
		epoch_start.tm_min = 0;
		epoch_start.tm_hour = 0;
		epoch_start.tm_mday = 1;
		epoch_start.tm_mon = 0;
		epoch_start.tm_year = 70;
		epoch_start.tm_isdst = -1;

		std::time_t basetime = std::mktime(&epoch_start);
		const auto retval = static_cast<long long int>(std::difftime(parTime, basetime));
		//assert(retval > 42ll * 365 * 24 * 60 * 60);
		return retval;
	}
} //namespace dindb
