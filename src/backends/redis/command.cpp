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
#include <hiredis/async.h>
#include <ciso646>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace redis {
	namespace {
	} //unnamed namespace

	Command::Command (std::string&& parAddress, uint16_t parPort) :
		m_conn(nullptr, &redisAsyncDisconnect),
		m_address(std::move(parAddress)),
		m_port(parPort)
	{
	}

	Command::~Command() noexcept {
	}

	void Command::connect() {
		if (not m_conn) {
			RedisConnection conn(
				redisAsyncConnect(m_address.c_str(), m_port),
				&redisAsyncDisconnect
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

	bool Command::is_connected() const {
		return m_conn and not m_conn->err;
	}

	auto Command::scan() -> scan_range {
		return scan_range(scan_iterator(this, false), scan_iterator(this, true));
	}

	auto Command::hscan (boost::string_ref parKey) -> hscan_range {
		return hscan_range(hscan_iterator(this, parKey, false), hscan_iterator(this, parKey, true));
	}

	auto Command::sscan (boost::string_ref parKey) -> sscan_range {
		return sscan_range(sscan_iterator(this, parKey, false), sscan_iterator(this, parKey, true));
	}

	auto Command::zscan (boost::string_ref parKey) -> zscan_range {
		return zscan_range(zscan_iterator(this, parKey, false), zscan_iterator(this, parKey, true));
	}

	Batch Command::make_batch() {
		assert(is_connected());
		return Batch(m_conn.get());
	}
} //namespace redis
