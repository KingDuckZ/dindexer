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

#include "reply.hpp"
#include <boost/variant/get.hpp>
#include <boost/lexical_cast.hpp>

namespace redis {
	const long long& get_integer (const Reply& parReply) {
		assert(RedisVariantType_Integer == parReply.which());
		return boost::get<long long>(parReply);
	}

	const std::string& get_string (const Reply& parReply) {
		assert(RedisVariantType_String == parReply.which());
		return boost::get<std::string>(parReply);
	}

	long long get_integer_autoconv_if_str (const Reply &parReply) {
		using boost::lexical_cast;

		const auto type = parReply.which();
		switch (type) {
		case RedisVariantType_Integer:
			return get_integer(parReply);
		case RedisVariantType_String:
			return lexical_cast<long long>(get_string(parReply));
		default:
			assert(false);
			return 0;
		}
	}

	const std::vector<Reply>& get_array (const Reply& parReply) {
		assert(RedisVariantType_Array == parReply.which());
		return boost::get<std::vector<Reply>>(parReply);
	}

	template <>
	const std::string& get<std::string> (const Reply& parReply) {
		return get_string(parReply);
	}

	template <>
	const std::vector<Reply>& get<std::vector<Reply>> (const Reply& parReply) {
		return get_array(parReply);
	}

	template <>
	const long long& get<long long> (const Reply& parReply) {
		return get_integer(parReply);
	}

	template const std::string& get<std::string> ( const Reply& parReply );
	template const std::vector<Reply>& get<std::vector<Reply>> ( const Reply& parReply );
	template const long long& get<long long> ( const Reply& parReply );
} //namespace redis
