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

namespace redis {
	const long long& get_integer (const RedisReplyType& parReply) {
		return boost::get<long long>(parReply);
	}

	const std::string& get_string (const RedisReplyType& parReply) {
		return boost::get<std::string>(parReply);
	}

	const std::vector<RedisReplyType>& get_array (const RedisReplyType& parReply) {
		return boost::get<std::vector<RedisReplyType>>(parReply);
	}

	template <>
	const std::string& get<std::string> (const RedisReplyType& parReply) {
		return get_string(parReply);
	}

	template <>
	const std::vector<RedisReplyType>& get<std::vector<RedisReplyType>> (const RedisReplyType& parReply) {
		return get_array(parReply);
	}

	template <>
	const long long& get<long long> (const RedisReplyType& parReply) {
		return get_integer(parReply);
	}

	template const std::string& get<std::string> ( const RedisReplyType& parReply );
	template const std::vector<RedisReplyType>& get<std::vector<RedisReplyType>> ( const RedisReplyType& parReply );
	template const long long& get<long long> ( const RedisReplyType& parReply );
} //namespace redis
