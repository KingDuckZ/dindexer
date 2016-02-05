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

#ifndef idBE93AF97FA4343ECA2BC8FB1FD3E5E60
#define idBE93AF97FA4343ECA2BC8FB1FD3E5E60

#include <cstdint>
#include <string>
#include <vector>

namespace mchlib {
	struct TigerHash {
		TigerHash ( void ) = default;

		union {
			struct {
				uint64_t part_a;
				uint64_t part_b;
				uint64_t part_c;
			};
			uint64_t data[3];
			uint8_t byte_data[sizeof(uint64_t) * 3];
		};
	};

	static_assert(sizeof(TigerHash) == 24, "Wrong struct size");

	void tiger_file ( const std::string& parPath, TigerHash& parHashFile, TigerHash& parHashDir, uint64_t& parSizeOut );
	void tiger_init_hash ( TigerHash& parHash );
	std::string tiger_to_string ( const TigerHash& parHash, bool parUpcase=false );
	void tiger_data ( const std::string& parData, TigerHash& parHash );
	void tiger_data ( const std::vector<char>& parData, TigerHash& parHash );
} //namespace mchlib

#endif
