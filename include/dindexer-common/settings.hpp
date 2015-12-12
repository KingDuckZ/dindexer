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

#ifndef idDC29E3C667BD4793BA0644AE7DC5BD3F
#define idDC29E3C667BD4793BA0644AE7DC5BD3F

#include <string>
#include <cstdint>

namespace din {
	struct DinDBSettings {
		std::string address;
		std::string username;
		std::string password;
		std::string dbname;
		uint16_t port;
	};

	//struct DinSettings {
	//};

	bool load_settings ( const std::string& parPath, DinDBSettings& parOut );
} //namespace din

#endif
