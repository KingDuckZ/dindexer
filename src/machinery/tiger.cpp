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

#include "dindexer-machinery/tiger.hpp"
#include "duckhandy/lexical_cast.hpp"
#include <fstream>
#include <cstdint>
#include <memory>
#include <cassert>
#include <algorithm>
#include <utility>
#include <sstream>
#include <iomanip>
#include <boost/utility/string_ref.hpp>

extern "C" void tiger ( const char* parStr, uint64_t parLength, uint64_t parHash[3], char parPadding );

#if defined(__SSE2__)
#	define USE_TIGER_SSE2
#endif

#if defined(USE_TIGER_SSE2)
extern "C" void tiger_sse2_chunk ( const char* parStr1, const char* parStr2, uint64_t parLength, uint64_t parRes1[3], uint64_t parRes2[3] );
extern "C" void tiger_sse2_last_chunk ( const char* parStr1, const char* parStr2, uint64_t parLength, uint64_t parRealLength1, uint64_t parRealLength2, uint64_t parRes1[3], uint64_t parRes2[3], char parPadding );
#else
extern "C" void tiger_2_chunk ( const char* parStr1, const char* parStr2, uint64_t parLength, uint64_t parRes1[3], uint64_t parRes2[3] );
extern "C" void tiger_2_last_chunk ( const char* parStr1, const char* parStr2, uint64_t parLength, uint64_t parRealLength1, uint64_t parRealLength2, uint64_t parRes1[3], uint64_t parRes2[3], char parPadding );
#endif

namespace mchlib {
	namespace {
		const uint32_t g_buff_size = 1024 * 8;
		const char g_tiger_padding = 0x80; //0x01 for V1

		uint64_t swap_long (uint64_t parNum) {
			parNum = (parNum & 0x00000000FFFFFFFF) << 32 | (parNum & 0xFFFFFFFF00000000) >> 32;
			parNum = (parNum & 0x0000FFFF0000FFFF) << 16 | (parNum & 0xFFFF0000FFFF0000) >> 16;
			parNum = (parNum & 0x00FF00FF00FF00FF) << 8  | (parNum & 0xFF00FF00FF00FF00) >> 8;
			return parNum;
		}
	} //unnamed namespace

	void tiger_init_hash (TigerHash& parHash) {
		parHash.part_a = 0x0123456789ABCDEFULL;
		parHash.part_b = 0xFEDCBA9876543210ULL;
		parHash.part_c = 0xF096A5B4C3B2E187ULL;
	}

	void tiger_file (const std::string& parPath, TigerHash& parHashFile, TigerHash& parHashDir, uint64_t& parSizeOut) {
		typedef decltype(std::declval<std::ifstream>().tellg()) FileSizeType;
		tiger_init_hash(parHashFile);

		std::ifstream src(parPath, std::ios::binary);
		src.exceptions(src.badbit); //Throw on read error
		src.seekg(0, std::ios_base::end);
		const auto file_size = src.tellg();
		src.seekg(0, std::ios_base::beg);

		const FileSizeType hash_size = (sizeof(TigerHash) + 63) & -64;
		const uint32_t buffsize = static_cast<uint32_t>(std::max(hash_size, std::min<FileSizeType>(file_size, g_buff_size)));
		std::unique_ptr<char[]> buff(new char[63 + buffsize]);
		char* const buff_ptr = reinterpret_cast<char*>((reinterpret_cast<std::intptr_t>(buff.get()) + 63) & (-64));
		assert(buff_ptr >= buff.get() and buff_ptr + buffsize <= buff.get() + 63 + buffsize);

		//Take a copy of parHashDir and work on it - if hashing fails at some
		//point, we need to leave the dir's hash untouched.
		auto hash_dir = parHashDir;

		//Use the initial value of the dir's hash as if it was part of the data to hash and start
		//by processing that value. Hash is reset to the initial value before the call to tiger.
		{
			std::copy(hash_dir.byte_data, hash_dir.byte_data + sizeof(hash_dir), buff_ptr);
			assert(hash_size >= static_cast<FileSizeType>(sizeof(hash_dir)));
			std::fill(buff_ptr + sizeof(hash_dir), buff_ptr + hash_size, 0);
			TigerHash dummy {};
			tiger_init_hash(hash_dir);
#if defined(USE_TIGER_SSE2)
			tiger_sse2_chunk(buff_ptr, buff_ptr, hash_size, dummy.data, hash_dir.data);
#else
			tiger_2_chunk(buff_ptr, buff_ptr, hash_size, dummy.data, hash_dir.data);
#endif
		}

		auto remaining = file_size;
		while (remaining > buffsize) {
			assert(buffsize >= sizeof(uint64_t) * 3);
			assert(buffsize == (buffsize & -64));
			assert(buffsize % 64 == 0);
			remaining -= buffsize;
			src.read(buff_ptr, buffsize);
#if defined(USE_TIGER_SSE2)
			tiger_sse2_chunk(buff_ptr, buff_ptr, buffsize, parHashFile.data, hash_dir.data);
#else
			tiger_2_chunk(buff_ptr, buff_ptr, buffsize, parHashFile.data, hash_dir.data);
#endif
		}

		{
			assert(remaining <= buffsize);
			src.read(buff_ptr, remaining);
			const auto aligned_size = remaining & -64;
			assert(aligned_size <= remaining);
			assert(aligned_size <= buffsize);
			const char* read_from_buff = buff_ptr;
			if (aligned_size) {
#if defined(USE_TIGER_SSE2)
				tiger_sse2_chunk(buff_ptr, buff_ptr, aligned_size, parHashFile.data, hash_dir.data);
#else
				tiger_2_chunk(buff_ptr, buff_ptr, aligned_size, parHashFile.data, hash_dir.data);
#endif
				assert((remaining & 63) == remaining - aligned_size);
				remaining -= aligned_size;
				read_from_buff += aligned_size;
			}

			//Remember to pass the augmented data size for the second reallength value: we passed the initial
			//dir's hash value (64 bytes) as if they were part of the data.
#if defined(USE_TIGER_SSE2)
			tiger_sse2_last_chunk(read_from_buff, read_from_buff, remaining, file_size, file_size + hash_size, parHashFile.data, hash_dir.data, g_tiger_padding);
#else
			tiger_2_last_chunk(read_from_buff, read_from_buff, remaining, file_size, file_size + hash_size, parHashFile.data, hash_dir.data, g_tiger_padding);
#endif
		}

		parSizeOut = static_cast<uint64_t>(file_size);
		parHashDir = hash_dir;
	}

	std::string tiger_to_string (const TigerHash& parHash, bool parUpcase) {
		std::ostringstream oss;
		if (parUpcase) {
			oss << std::uppercase;
		}
		oss << std::hex << std::setfill('0') << std::setw(2 * sizeof(uint64_t))
			<< swap_long(parHash.part_a)
			<< std::hex << std::setfill('0') << std::setw(2 * sizeof(uint64_t))
			<< swap_long(parHash.part_b)
			<< std::hex << std::setfill('0') << std::setw(2 * sizeof(uint64_t))
			<< swap_long(parHash.part_c)
		;
		return oss.str();
	}

	TigerHash string_to_tiger (const std::string& parString) {
		using boost::string_ref;
		using TigerPartType = decltype(TigerHash::part_a);

		assert(parString.size() == sizeof(TigerHash) * 2);

		TigerHash retval;
		const string_ref inp(parString);
		retval.part_a = swap_long(dhandy::lexical_cast<TigerPartType, dhandy::tags::hex>(inp.substr(0, sizeof(TigerPartType) * 2)));
		retval.part_b = swap_long(dhandy::lexical_cast<TigerPartType, dhandy::tags::hex>(inp.substr(sizeof(TigerPartType) * 2, sizeof(TigerPartType) * 2)));
		retval.part_c = swap_long(dhandy::lexical_cast<TigerPartType, dhandy::tags::hex>(inp.substr(sizeof(TigerPartType) * 4, sizeof(TigerPartType) * 2)));
		return retval;
	}

	void tiger_data (const std::string& parData, TigerHash& parHash) {
		tiger (parData.data(), parData.size(), parHash.data, g_tiger_padding);
	}

	void tiger_data (const std::vector<char>& parData, TigerHash& parHash) {
		tiger (parData.data(), parData.size(), parHash.data, g_tiger_padding);
	}
} //namespace mchlib

#if defined(USE_TIGER_SSE2)
#	undef USE_TIGER_SSE2
#endif
