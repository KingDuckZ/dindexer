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

#ifndef id3CD7F105AC314540A864487E981E5A7E
#define id3CD7F105AC314540A864487E981E5A7E

#include "tiger.hpp"
#include <string>
#include <boost/utility/string_ref.hpp>
#include <cstdint>
#include <ctime>
#include <boost/flyweight.hpp>
#include <boost/flyweight/no_locking.hpp>

namespace mchlib {
	struct FileRecordData {
		struct MimeStringTagStruct { };
		typedef boost::flyweights::tag<MimeStringTagStruct> MimeStringTag;
		typedef boost::flyweight<std::string, boost::flyweights::no_locking, MimeStringTag> mime_string;

		FileRecordData ( void ) = default;
		FileRecordData ( const char* parPath, std::time_t parATime, std::time_t parMTime, uint64_t parLevel, bool parIsDir, bool parIsSymLink ) :
			hash {},
			abs_path(parPath),
			mime_full(),
			atime(parATime),
			mtime(parMTime),
			mime_type(),
			mime_charset(),
			size(0),
			level(parLevel),
			path_offset(0),
			is_directory(parIsDir),
			is_symlink(parIsSymLink),
			unreadable(false),
			hash_valid(false)
		{
		}

		FileRecordData ( std::string&& parPath, uint16_t parRelPathOffs, std::time_t parATime, std::time_t parMTime, uint16_t parLevel, bool parIsDir, bool parIsSymLink ) :
			hash {},
			abs_path(std::move(parPath)),
			mime_full(),
			atime(parATime),
			mtime(parMTime),
			mime_type(),
			mime_charset(),
			size(0),
			level(parLevel),
			path_offset(parRelPathOffs),
			is_directory(parIsDir),
			is_symlink(parIsSymLink),
			unreadable(false),
			hash_valid(false)
	{
		assert(path_offset <= abs_path.size());
	}

#if defined(NDEBUG)
		FileRecordData ( const FileRecordData& ) = delete;
#else
		FileRecordData ( const FileRecordData& ) = default;
#endif
		FileRecordData ( FileRecordData&& ) = default;
		FileRecordData& operator= ( const FileRecordData& ) = delete;
		FileRecordData& operator= ( FileRecordData&& ) = default;
#if !defined(NDEBUG)
		bool operator== ( const FileRecordData& parOther ) const;
#endif

		boost::string_ref path ( void ) const { return boost::string_ref(abs_path).substr(path_offset); }

		TigerHash hash;
		std::string abs_path;
		mime_string mime_full;
		std::time_t atime;
		std::time_t mtime;
		boost::string_ref mime_type;
		boost::string_ref mime_charset;
		uint64_t size;
		uint16_t level;
		uint16_t path_offset; //Relative path starting character into abs_path
		bool is_directory;
		bool is_symlink;
		bool unreadable;
		bool hash_valid;
	};

	struct SetRecordData {
		boost::string_ref name;
		char type;
		char content_type;
	};

	struct SetRecordDataFull : public SetRecordData {
		std::string name;
		uint32_t disk_number;
	};

#if !defined(NDEBUG)
	inline bool FileRecordData::operator== (const FileRecordData& parOther) const {
		return (this->abs_path == parOther.abs_path);
	}
#endif
} //namespace mchlib

#endif
