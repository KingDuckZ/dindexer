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

#include "command.hpp"
#include <hiredis/hiredis.h>
#include <ciso646>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/variant/get.hpp>

namespace redis {
	namespace {
		using RedisReply = std::unique_ptr<redisReply, void(*)(void*)>;

		RedisReplyType make_redis_reply_type (redisReply* parReply) {
			using boost::transform_iterator;
			using PtrToReplyIterator = transform_iterator<RedisReplyType(*)(redisReply*), redisReply**>;

			switch (parReply->type) {
			case REDIS_REPLY_INTEGER:
				return parReply->integer;
			case REDIS_REPLY_STRING:
				return std::string(parReply->str, parReply->len);
			case REDIS_REPLY_ARRAY:
				return std::vector<RedisReplyType>(
					PtrToReplyIterator(parReply->element, &make_redis_reply_type),
					PtrToReplyIterator(parReply->element + parReply->elements, &make_redis_reply_type)
				);
			default:
				return RedisReplyType();
			};
		}
	} //unnamed namespace

	long long get_integer (const RedisReplyType& parReply) {
		return boost::get<long long>(parReply);
	}

	std::string get_string (const RedisReplyType& parReply) {
		return boost::get<std::string>(parReply);
	}

	std::vector<RedisReplyType> get_array (const RedisReplyType& parReply) {
		return boost::get<std::vector<RedisReplyType>>(parReply);
	}

	Command::Command (std::string&& parAddress, uint16_t parPort, bool parConnect) :
		m_conn(nullptr, &redisFree),
		m_address(std::move(parAddress)),
		m_port(parPort)
	{
		if (parConnect)
			this->connect();
	}

	Command::~Command() noexcept {
	}

	void Command::connect() {
		if (not m_conn) {
			struct timeval timeout = {5, 500000}; //5.5 seconds?
			RedisConnection conn(
				redisConnectWithTimeout(m_address.c_str(), m_port, timeout),
				&redisFree
			);
			if (not conn) {
				std::ostringstream oss;
				oss << "Unable to connect to Redis server at " << m_address << ':' << m_port;
				throw std::runtime_error(oss.str());
			}
			if (conn->err) {
				std::ostringstream oss;
				oss << "Unable to connect to Redis server at " << m_address << ':' << m_port <<
					": " << conn->errstr;
				throw std::runtime_error(oss.str());
			}
			std::swap(conn, m_conn);
		}
	}

	void Command::disconnect() {
		m_conn.reset();
	}

	RedisReplyType Command::run_pvt (const char* parCommand, int parArgc, const char** parArgv, std::size_t* parLengths) {
		assert(parCommand);
		assert(parArgv);
		assert(parLengths); //This /could/ be null, but I don't see why it should
		assert(is_connected());

		RedisReply reply(
			static_cast<redisReply*>(redisCommandArgv(m_conn.get(), parArgc, parArgv, parLengths)),
			&freeReplyObject
		);

		return make_redis_reply_type(reply.get());

		//std::string key;
		//{
		//	std::ostringstream key_oss;
		//	RedisReply incr_reply(static_cast<redisReply*>(redisCommand(m_conn.get(), "incr set_counter")), &freeReplyObject);
		//	key_oss << "sets:" << incr_reply->integer;
		//	key = key_oss.str();
		//}

		//RedisReply insert_reply(
		//	static_cast<redisReply*>(redisCommand(
		//		m_conn.get(),
		//		"hmset %b name %b disk_label %b fs_uuid %b",
		//		key.data(),
		//		key.size(),
		//		parSetData.name.data(),
		//		parSetData.name.size(),
		//		parSetData.disk_label.data(),
		//		parSetData.disk_label.size(),
		//		parSetData.fs_uuid.data(),
		//		parSetData.fs_uuid.size()
		//	)),
		//	&freeReplyObject
		//);
	}

	bool Command::is_connected() const {
		return m_conn and not m_conn->err;
	}
} //namespace redis
