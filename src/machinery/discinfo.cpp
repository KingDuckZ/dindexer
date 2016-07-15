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

#include "discinfo.hpp"
#include "pathname.hpp"
#include "duckhandy/lengthof.h"
#include <map>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <ciso646>
#if defined(WITH_MEDIA_AUTODETECT)
#	include <blkid/blkid.h>
#endif
#include <linux/limits.h>
#include <sys/stat.h>
#include <memory>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <scsi/sg.h>
#if !defined(NDEBUG)
#	include <iostream>
#endif
#include <cstdint>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range_core.hpp>

namespace fs = boost::filesystem;

namespace mchlib {
	namespace {
		using MountpointsMap = std::map<std::string, std::string>;

#if defined(WITH_MEDIA_AUTODETECT)
		//See: http://lkml.iu.edu/hypermail/linux/kernel/0602.1/1295.html
		enum LinuxDeviceTypes {
			LinuxDeviceType_Disk = 0x00,
			LinuxDeviceType_Tape = 0x01,
			LinuxDeviceType_Printer = 0x02,
			LinuxDeviceType_Processor = 0x03,
			LinuxDeviceType_Worm = 0x04,
			LinuxDeviceType_ROM = 0x05,
			LinuxDeviceType_Scanner = 0x06,
			LinuxDeviceType_Mod = 0x07,
			LinuxDeviceType_MediumChanger = 0x08,
			LinuxDeviceType_Comm = 0x09,
			LinuxDeviceType_Raid = 0x0c,
			LinuxDeviceType_Enclosure = 0x0d,
			LinuxDeviceType_RBC = 0x0e,
			LinuxDeviceType_NoLun = 0x7f,
			LinuxDeviceType_Unknown = 0xff
		};

		//From cdrecord's source (cdrecord/scsi_mmc.c)
		enum DiscTypes {
			DiscType_Reserved = 0x0000, //"Reserved"
			DiscType_NonRemovable = 0x0001, //"Non -removable Disk"
			DiscType_Removable = 0x0002, //"Removable Disk"
			DiscType_MOErasable = 0x0003, //"MO Erasable"
			DiscType_MOWriteOnce = 0x0004, //"MO Write Once"
			DiscType_ASMO = 0x0005, //"AS-MO"

			/* 0x06..0x07 is reserved */

			DiscType_CDROM = 0x0008, //"CD-ROM"
			DiscType_CDR = 0x0009, //"CD-R"
			DiscType_CDRW = 0x000A, //"CD-RW"

			/* 0x0B..0x0F is reserved */

			DiscType_DVDROM = 0x0010, //"DVD-ROM"
			DiscType_DVDR = 0x0011, //"DVD-R sequential recording"
			DiscType_DVDRAM = 0x0012, //"DVD-RAM"
			DiscType_DVDRWRestricted = 0x0013, //"DVD-RW restricted overwrite"
			DiscType_DVDRW = 0x0014, //"DVD-RW sequential recording"
			DiscType_DVDRDL = 0x0015, //"DVD-R/DL sequential recording"
			DiscType_DVDRDLJump = 0x0016, //"DVD-R/DL layer jump recording"
			DiscType_DVDRWDL = 0x0017, //"DVD-RW/DL"

			/* 0x18..0x19 is reserved */

			DiscType_DVDPRW = 0x001A, //"DVD+RW"
			DiscType_DVDPR = 0x001B, //"DVD+R"

			DiscType_DDCDROM = 0x0020, //"DDCD-ROM"
			DiscType_DDCDR = 0x0021, //"DDCD-R"
			DiscType_DDCDRW = 0x0022, //"DDCD-RW"

			DiscType_DVDPRWDL = 0x002A, //"DVD+RW/DL"
			DiscType_DVDPRDL = 0x002B, //"DVD+R/DL"

			DiscType_BDROM = 0x0040, //"BD-ROM"
			DiscType_BDR = 0x0041, //"BD-R sequential recording"
			DiscType_BDRRandom = 0x0042, //"BD-R random recording"
			DiscType_BDRE = 0x0043, //"BD-RE"

			/* 0x44..0x4F is reserved */

			DiscType_HDDVDROM = 0x0050, //"HD DVD-ROM"
			DiscType_HDDVDR = 0x0051, //"HD DVD-R"
			DiscType_HDDVDRAM = 0x0052, //"HD DVD-RAM"
			DiscType_HDDVDRW = 0x0053, //"HD DVD-RW"

			/* 0x54..0x57 is reserved */

			DiscType_HDDVDRDL = 0x0058, //"HD DVD-R/DL"
			DiscType_HDDVDRWDL = 0x005A, //"HD DVD-RW/DL"

			DiscType_NoStandardProfile = 0xFFFF, //"No standard Profile"
		};
#endif

		MountpointsMap list_mountpoints() {
			std::ifstream rd("/proc/mounts");
			std::string line;
			MountpointsMap retmap;
			boost::char_separator<char> sep(" ");

			while (std::getline(rd, line)) {
				boost::tokenizer<boost::char_separator<char>> tok(line, sep);
				auto it_token = tok.begin();

				std::string dev(*it_token);
				++it_token;
				std::string mount(*it_token);

				retmap[std::move(mount)] = std::move(dev);
			}

			return retmap;
		}

#if defined(WITH_MEDIA_AUTODETECT)
		LinuxDeviceTypes get_dev_type (const std::string& parDev) {
			const char dev_prefix[] = "/dev/";
			const std::size_t dev_prefix_len = lengthof(dev_prefix) - 1;
			int retval = LinuxDeviceType_Unknown;

			struct stat st;
			if (stat(parDev.c_str(), &st) or not S_ISBLK(st.st_mode)) {
				return LinuxDeviceType_Unknown;
			}

			if (parDev.size() > dev_prefix_len and std::equal(dev_prefix, dev_prefix + dev_prefix_len, parDev.begin())) {
				std::unique_ptr<char[]> dev_buff(new char[PATH_MAX + 1]);
				dev_t disk = 0;
				if (blkid_devno_to_wholedisk(st.st_rdev, dev_buff.get(), PATH_MAX + 1, &disk)) {
					return LinuxDeviceType_Unknown;
				}

				std::string sys_type_path("/sys/block/");
				sys_type_path += dev_buff.get();
				sys_type_path += "/device/type";

				std::ifstream rd(sys_type_path);
				rd >> retval;
			}
			return static_cast<LinuxDeviceTypes>(retval);
		}
#endif

		std::string find_with_same_inode (const std::string& parWhat, const char* parWhere) {
			using boost::make_iterator_range;

			struct stat st;
			if (stat(parWhat.c_str(), &st))
				return std::string();

			const auto& inode = st.st_ino;

			fs::path p(parWhere);
			if (not fs::exists(p) or not fs::is_directory(p)) {
				throw std::runtime_error(
					std::string("Search path \"") + p.string() +
					"\" doesn't exist");
			}

			for (const fs::directory_entry& itm : make_iterator_range(fs::directory_iterator(p), fs::directory_iterator())) {
				struct stat curr_st;
				if (not stat(itm.path().c_str(), &curr_st) and inode == curr_st.st_ino)
					return fs::basename(itm);
			}

			return std::string();
		}

		//Get disc label by doing the equivalent of:
		//find -L /dev/disk/by-label -inum $(stat -c %i /dev/sda1) -print
		std::string retrieve_label (const std::string& parDev) {
			return find_with_same_inode(parDev, "/dev/disk/by-label");
		}

		std::string retrieve_uuid (const std::string& parDev) {
			return find_with_same_inode(parDev, "/dev/disk/by-uuid");
		}
	} //unnamed namespace

	DiscInfo::DiscInfo (std::string&& parPath) :
		m_initial_path(std::move(parPath)),
		m_mountpoint(),
		m_device()
	{
		PathName input_path((std::string(m_initial_path)));
		auto mounts = list_mountpoints();

		do {
			auto curr_path = input_path.path();
			auto it_found = mounts.find(curr_path);
			if (mounts.end() != it_found) {
				m_mountpoint = std::move(curr_path);
				m_device = it_found->second;
				break;
			}
			input_path.pop_right();
		} while(input_path.atom_count() > 0);

		if (mountpoint_found()) {
			m_label = retrieve_label(m_device);
			m_uuid = retrieve_uuid(m_device);
		}
	}

	bool DiscInfo::mountpoint_found() const {
		return not m_device.empty();
	}

#if defined(WITH_MEDIA_AUTODETECT)
	DriveTypes DiscInfo::drive_type() const {
		auto dev_type = get_dev_type(m_device);
		switch (dev_type) {
		case LinuxDeviceType_Disk:
			return DriveType_HardDisk;
		case LinuxDeviceType_ROM:
			return DriveType_Optical;
		case LinuxDeviceType_Tape:
		case LinuxDeviceType_Printer:
		case LinuxDeviceType_Processor:
		case LinuxDeviceType_Worm:
		case LinuxDeviceType_Scanner:
		case LinuxDeviceType_Mod:
		case LinuxDeviceType_MediumChanger:
		case LinuxDeviceType_Comm:
		case LinuxDeviceType_Raid:
		case LinuxDeviceType_Enclosure:
		case LinuxDeviceType_RBC:
		case LinuxDeviceType_NoLun:
			return DriveType_Other;
		case LinuxDeviceType_Unknown:
		default:
			return DriveType_Unknown;
		}
	}

	OpticalTypes DiscInfo::optical_type() const {
		struct FileDesc {
			FileDesc ( void ) : m_fd(0) {}
			FileDesc ( int parFD ) : m_fd(parFD) {}
			FileDesc ( std::nullptr_t ) : m_fd(0) {}
			operator int() { return m_fd; }
			bool operator== ( const FileDesc& parOther ) const { return m_fd == parOther.m_fd; }
			bool operator!= ( const FileDesc& parOther ) const { return m_fd != parOther.m_fd; }
			int m_fd;
		};
		struct FileCloser {
			typedef FileDesc pointer;
			void operator() ( FileDesc parFile ) const { close(parFile.m_fd); }
		};
		using AutoFile = std::unique_ptr<FileDesc, FileCloser>;

		const auto drive_type = this->drive_type();
		if (drive_type != DriveType_Optical) {
			return OpticalType_NotOptical;
		}

		char out_data[8] {};
		unsigned char scsi_command[10] = {'F', '\0', '\0', '\0', '\0', '\0', '\0', '\0', lengthof(out_data), '\0'};
		unsigned char sense_buffer[16] {};
		AutoFile f_dev(open(m_device.c_str(), O_RDONLY));
		sg_io_hdr_t sg_io {};
		//See: http://www.tldp.org/HOWTO/SCSI-Generic-HOWTO/sg_io_hdr_t.html
		sg_io.interface_id = 'S';
		sg_io.dxfer_direction = SG_DXFER_FROM_DEV;
		sg_io.dxfer_len = lengthof(out_data);
		sg_io.dxferp = out_data;
		sg_io.timeout = 40000;
		sg_io.flags |= SG_FLAG_DIRECT_IO;
		sg_io.mx_sb_len = lengthof(sense_buffer);
		sg_io.cmdp = scsi_command;
		sg_io.cmd_len = lengthof(scsi_command);
		sg_io.sbp = sense_buffer;
		const int ioctl_ret = ioctl(f_dev.get(), SG_IO, &sg_io);
#if !defined(NDEBUG)
		switch (ioctl_ret) {
		case EBADF:
			std::cerr << "fd is not a valid descriptor.\n";
			return OpticalType_Unknown;
		case EFAULT:
			std::cerr << "argp references an inaccessible memory area.\n";
			return OpticalType_Unknown;
		case EINVAL:
			std::cerr << "request or argp is not valid.\n";
			return OpticalType_Unknown;
		case ENOTTY:
			std::cerr << "fd is not associated with a character special device.\n";
			return OpticalType_Unknown;
		}
#else
		if (ioctl_ret) {
			return OpticalType_Unknown;
		}
#endif

		const DiscTypes detected_type = static_cast<DiscTypes>(
			static_cast<uint16_t>((static_cast<uint16_t>(out_data[7]) & 0xFF)) | (static_cast<uint16_t>(out_data[6]) << 8 & 0xFF00)
		);
		switch (detected_type) {
		case DiscType_NonRemovable: //"Non -removable Disk"
		case DiscType_Removable: //"Removable Disk"
		case DiscType_MOErasable: //"MO Erasable"
		case DiscType_MOWriteOnce: //"MO Write Once"
		case DiscType_ASMO: //"AS-MO"
			return OpticalType_NotOptical;
		case DiscType_CDROM: //"CD-ROM"
		case DiscType_CDR: //"CD-R"
		case DiscType_CDRW: //"CD-RW"
			return OpticalType_CDRom;
		case DiscType_DVDROM: //"DVD-ROM"
		case DiscType_DVDR: //"DVD-R sequential recording"
		case DiscType_DVDRAM: //"DVD-RAM"
		case DiscType_DVDRWRestricted: //"DVD-RW restricted overwrite"
		case DiscType_DVDRW: //"DVD-RW sequential recording"
		case DiscType_DVDRDL: //"DVD-R/DL sequential recording"
		case DiscType_DVDRDLJump: //"DVD-R/DL layer jump recording"
		case DiscType_DVDRWDL: //"DVD-RW/DL"
		case DiscType_DVDPRW: //"DVD+RW"
		case DiscType_DVDPR: //"DVD+R"
		case DiscType_DVDPRWDL: //"DVD+RW/DL"
		case DiscType_DVDPRDL: //"DVD+R/DL"
			return OpticalType_DVD;
		case DiscType_DDCDROM: //"DDCD-ROM"
		case DiscType_DDCDR: //"DDCD-R"
		case DiscType_DDCDRW: //"DDCD-RW"
			return OpticalType_DDCDRom;
		case DiscType_BDROM: //"BD-ROM"
		case DiscType_BDR: //"BD-R sequential recording"
		case DiscType_BDRRandom: //"BD-R random recording"
		case DiscType_BDRE: //"BD-RE"
			return OpticalType_BluRay;
		case DiscType_HDDVDROM: //"HD DVD-ROM"
		case DiscType_HDDVDR: //"HD DVD-R"
		case DiscType_HDDVDRAM: //"HD DVD-RAM"
		case DiscType_HDDVDRW: //"HD DVD-RW"
		case DiscType_HDDVDRDL: //"HD DVD-R/DL"
		case DiscType_HDDVDRWDL: //"HD DVD-RW/DL"
			return OpticalType_DVD;
		case DiscType_Reserved: //"Reserved"
		case DiscType_NoStandardProfile: //"No standard Profile"
		default:
			return OpticalType_Unknown;
		};
	}
#endif

	const std::string& DiscInfo::label() const {
		return m_label;
	}

	const std::string& DiscInfo::filesystem_uuid() const {
		return m_uuid;
	}
} //namespace mchlib
