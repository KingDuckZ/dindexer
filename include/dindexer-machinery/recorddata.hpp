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
#include <functional>

namespace mchlib {
	struct FileRecordData {
		struct MimeStringTagStruct { };
		typedef boost::flyweights::tag<MimeStringTagStruct> MimeStringTag;
		typedef boost::flyweight<std::string, boost::flyweights::no_locking, MimeStringTag> mime_string;

		FileRecordData ( void ) = default;
		FileRecordData ( const char* parPath, std::time_t parATime, std::time_t parMTime, uint16_t parLevel, bool parIsDir, bool parIsSymLink ) :
			hash {},
			abs_path(parPath),
			mime_full(),
			atime(parATime),
			mtime(parMTime),
			size(0),
			level(parLevel),
			path_offset(0),
			mime_type_offset(0),
			mime_type_length(0),
			mime_charset_offset(0),
			mime_charset_length(0),
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
			size(0),
			level(parLevel),
			path_offset(parRelPathOffs),
			mime_type_offset(0),
			mime_type_length(0),
			mime_charset_offset(0),
			mime_charset_length(0),
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

		boost::string_ref path ( void ) const {
			assert(abs_path.data());
			return boost::string_ref(abs_path).substr(path_offset);
		}

		boost::string_ref mime_type ( void ) const {
			assert(mime_full.get().data());
			return boost::string_ref(mime_full.get()).substr(mime_type_offset, mime_type_length);
		}

		boost::string_ref mime_charset ( void ) const {
			assert(mime_full.get().data());
			return boost::string_ref(mime_full.get()).substr(mime_charset_offset, mime_charset_length);
		}

		void set_mime_parts ( boost::string_ref parType, boost::string_ref parCharset ) {
			const auto& mime = mime_full.get();
			{
				assert(std::less_equal<const char*>()(mime.data(), parType.data()));
				assert(std::less_equal<const char*>()(parType.data() + parType.size(), mime.data() + mime.size()));
				assert(parType.data() - mime.data() < USHRT_MAX);
				assert(parType.size() < USHRT_MAX);
				assert(parType.size() + (parType.data() - mime.data()) <= mime.size());
				mime_type_offset = static_cast<uint16_t>(parType.data() - mime.data());
				mime_type_length = static_cast<uint16_t>(parType.size());
			}
			{
				assert(std::less_equal<const char*>()(mime.data(), parCharset.data()));
				assert(std::less_equal<const char*>()(parCharset.data() + parCharset.size(), mime.data() + mime.size()));
				assert(parCharset.data() - mime.data() < USHRT_MAX);
				assert(parCharset.size() < USHRT_MAX);
				assert(parCharset.size() + (parCharset.data() - mime.data()) <= mime.size());
				mime_charset_offset = static_cast<uint16_t>(parCharset.data() - mime.data());
				mime_charset_length = static_cast<uint16_t>(parCharset.size());
			}
		}

		TigerHash hash;
		std::string abs_path;
		mime_string mime_full;
		std::time_t atime;
		std::time_t mtime;
		uint64_t size;
		uint16_t level;
		uint16_t path_offset; //Relative path starting character into abs_path
		uint16_t mime_type_offset; //Mime type starting character into mime_full
		uint16_t mime_type_length; //Mime type string length
		uint16_t mime_charset_offset; //Mime charset starting character into mime_full
		uint16_t mime_charset_length; //Mime charset string length
		bool is_directory;
		bool is_symlink;
		bool unreadable;
		bool hash_valid;
	};

	struct SetRecordDataFull {
		std::string name;
		std::string disk_label;
		std::string fs_uuid;
		uint32_t disk_number;
		char type;
		char content_type;
	};

#if !defined(NDEBUG)
	inline bool FileRecordData::operator== (const FileRecordData& parOther) const {
		return (this->abs_path == parOther.abs_path);
	}
#endif
} //namespace mchlib

#endif
