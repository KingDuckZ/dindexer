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

#include "dindexer-machinery/mediatypes.hpp"
#include <map>
#include <stdexcept>

namespace mchlib {
	const std::string& media_type_to_str (MediaTypes parType) {
		static const std::map<MediaTypes, const std::string> types {
			{MediaType_CDRom, "CD-Rom"},
			{MediaType_Directory, "Directory"},
			{MediaType_DVD, "DVD"},
			{MediaType_BluRay, "BluRay"},
			{MediaType_FloppyDisk, "Floppy Disk"},
			{MediaType_HardDisk, "Hard Disk"},
			{MediaType_IomegaZip, "Iomega Zip"},
			{MediaType_Other, "Other"}
		};

		auto it_ret = types.find(parType);
		if (types.end() == it_ret) {
			const char err[2] = {static_cast<char>(parType), '\0'};
			throw std::out_of_range(err);
		}
		return it_ret->second;
	}

	MediaTypes char_to_media_type (char parMType) {
		return static_cast<MediaTypes>(parMType);
	}

	char media_type_to_char (MediaTypes parMType) {
		return static_cast<char>(parMType);
	}
} //namespace mchlib
