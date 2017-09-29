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

#include "create_tables.hpp"
#include "create_tables_query.hpp"
#include "pq/connection.hpp"
#include "backend_postgresql_config.h"
#if defined(ZLIB_FOUND)
#	include <boost/iostreams/device/array.hpp>
#	include <boost/iostreams/stream_buffer.hpp>
#	include <boost/iostreams/filter/gzip.hpp>
#	include <boost/iostreams/filtering_stream.hpp>
#	include <algorithm>
#	include <iterator>
#	include <iostream>
#endif
#include <string>
#include <cassert>

namespace dindb {
	void create_tables (pq::Connection& parConn) {
		assert(parConn.is_connected());

#if defined(ZLIB_FOUND)
		using boost::iostreams::filtering_istream;
		using boost::iostreams::stream_buffer;
		using boost::iostreams::array_source;
		using boost::iostreams::gzip_decompressor;
		using std::istream_iterator;
		using std::string;

		filtering_istream fs;
		stream_buffer<array_source> text_stream(reinterpret_cast<const char*>(create_tables_query), create_tables_query_len);
		fs.push(gzip_decompressor{});
		fs.push(text_stream);

		string query_str;
		query_str.reserve(create_tables_query_len);
		std::copy(
			istream_iterator<unsigned char>(fs >> std::noskipws),
			istream_iterator<unsigned char>(),
			std::back_inserter(query_str)
		);

#else
		std::string query_str(reinterpret_cast<const char*>(create_tables_query), create_tables_query_len);
#endif
		std::cout << query_str << std::endl;

		parConn.query(query_str);
	}
} //namespace dindb
