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

#ifndef idED32F82A21AE430EA2E253EC308F872C
#define idED32F82A21AE430EA2E253EC308F872C

#include <string>

namespace din {
#if defined(WITH_MEDIA_AUTODETECT)
	enum OpticalTypes {
		OpticalType_DVD,
		OpticalType_CDRom,
		OpticalType_DDCDRom,
		OpticalType_BluRay,
		OpticalType_NotOptical,
		OpticalType_Unknown
	};

	enum DriveTypes {
		DriveType_Optical,
		DriveType_HardDisk,
		DriveType_Other,
		DriveType_Unknown
	};
#endif

	class DiscInfo {
	public:
		explicit DiscInfo ( std::string&& parPath );
		~DiscInfo ( void ) noexcept = default;

		const std::string& mountpoint ( void ) const { return m_mountpoint; }
		const std::string& device ( void ) const { return m_device; }
		const std::string& original_path ( void ) const { return m_initial_path; }
		bool mountpoint_found ( void ) const;
#if defined(WITH_MEDIA_AUTODETECT)
		OpticalTypes optical_type ( void ) const;
		DriveTypes drive_type ( void ) const;
#endif

	private:
		const std::string m_initial_path;
		std::string m_mountpoint;
		std::string m_device;
	};
} //namespace din

#endif
