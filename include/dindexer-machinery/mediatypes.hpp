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

#ifndef id700AFD0F33634ACC88079BB8853A9E13
#define id700AFD0F33634ACC88079BB8853A9E13

#include "helpers/compatibility.h"
#include <string>

namespace mchlib {
	enum MediaTypes {
		MediaType_CDRom = 'C',
		MediaType_Directory = 'D',
		MediaType_DVD = 'V',
		MediaType_BluRay = 'B',
		MediaType_FloppyDisk = 'F',
		MediaType_HardDisk = 'H',
		MediaType_IomegaZip = 'Z',
		MediaType_Other = 'O'
	};

	const std::string& media_type_to_str ( MediaTypes parType );
	MediaTypes char_to_media_type ( char parMType ) a_pure;
	char media_type_to_char ( MediaTypes parMType ) a_pure;
} //namespace mchlib

#endif
