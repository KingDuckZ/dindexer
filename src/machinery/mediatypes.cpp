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
#include "machineryConfig.h"
#include <stdexcept>

namespace mchlib {
	namespace {
#if defined(WITH_NICE_MEDIA_TYPES)
		constexpr const char* get_enum_desc (MediaTypes parType) {
			switch (parType) {
			case MediaTypes::CDRom: return "CD-Rom";
			case MediaTypes::Directory: return "Directory";
			case MediaTypes::DVD: return "DVD";
			case MediaTypes::BluRay: return "BluRay";
			case MediaTypes::FloppyDisk: return "Floppy Disk";
			case MediaTypes::HardDisk: return "Hard Disk";
			case MediaTypes::IomegaZip: return "Iomega Zip";
			case MediaTypes::Other: return "Other";
			}
			return "needed for gcc 5";
		}

		constexpr auto g_descriptions = ::better_enums::make_map(get_enum_desc);
#endif
	} //unnamed namespace

	std::string media_type_to_str (MediaTypes parType) {
		try {
#if defined(WITH_NICE_MEDIA_TYPES)
			return std::string(g_descriptions[parType]);
#else
			return std::string(parType._to_string());
#endif
		}
		catch (const std::runtime_error&) {
			const char err[2] = {static_cast<char>(parType), '\0'};
			throw std::out_of_range(err);
		}
	}

	MediaTypes char_to_media_type (char parMType) {
		return MediaTypes::_from_integral(parMType);
	}

	char media_type_to_char (MediaTypes parMType) {
		return parMType._to_integral();
	}
} //namespace mchlib
