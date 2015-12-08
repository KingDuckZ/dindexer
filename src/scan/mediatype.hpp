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

#ifndef id66D41389BC59433CA58E325395A6197B
#define id66D41389BC59433CA58E325395A6197B

#include <string>
#include <stdexcept>

namespace din {
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

#if defined(WITH_MEDIA_AUTODETECT)
	MediaTypes guess_media_type ( std::string&& parPath );

	class UnknownMediaTypeException : std::runtime_error {
	public:
		UnknownMediaTypeException ( const std::string& parWhat );
		UnknownMediaTypeException ( const char* parWhat );
	};

	class CantAutodetectException : std::runtime_error {
	public:
		CantAutodetectException ( const std::string& parWhat );
		CantAutodetectException ( const char* parWhat );
	};
#endif
} //namespace din

#endif
