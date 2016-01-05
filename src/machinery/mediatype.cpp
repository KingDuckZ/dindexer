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

#include "dindexer-machinery/mediatype.hpp"
#include "pathname.hpp"
#include "discinfo.hpp"
#include <utility>

namespace din {
#if defined(WITH_MEDIA_AUTODETECT)
	UnknownMediaTypeException::UnknownMediaTypeException (const std::string& parWhat) :
		std::runtime_error(parWhat)
	{
	}
	UnknownMediaTypeException::UnknownMediaTypeException (const char* parWhat) :
		std::runtime_error(parWhat)
	{
	}

	CantAutodetectException::CantAutodetectException (const std::string& parWhat) :
		std::runtime_error(parWhat)
	{
	}
	CantAutodetectException::CantAutodetectException (const char* parWhat) :
		std::runtime_error(parWhat)
	{
	}

	dinlib::MediaTypes guess_media_type (std::string&& parPath) {
		DiscInfo info(std::move(parPath));
		const DriveTypes drive_type = info.drive_type();
		if (DriveType_HardDisk == drive_type) {
			if (info.mountpoint() == PathName(info.original_path()).path())
				return dinlib::MediaType_HardDisk;
			else
				return dinlib::MediaType_Directory;
		}
		else if (DriveType_Optical == drive_type) {
			switch (info.optical_type()) {
			case OpticalType_DVD:
				return dinlib::MediaType_DVD;
			case OpticalType_CDRom:
				return dinlib::MediaType_CDRom;
			case OpticalType_BluRay:
				return dinlib::MediaType_BluRay;
			default:
				throw UnknownMediaTypeException("Set autodetect failed because this media type is unknown, please specify the set type manually");
			}
		}
		else {
			throw CantAutodetectException("Can't autodetect set type, please specify it manually");
		}
	}
#endif
} //namespace din
