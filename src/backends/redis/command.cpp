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
#include "helpers/lexical_cast.hpp"
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <ciso646>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#if defined(WITH_CRYPTOPP)
#	include <crypto++/sha.h>
#endif

namespace redis {
	namespace {
#if defined(WITH_CRYPTOPP)
		struct LuaScriptHash {
			union {
				struct {
					uint64_t part_a, part_b;
					uint32_t part_c;
				};
				uint8_t raw_bytes[20];
			};
		};
#endif
	} //unnamed namespace

	void on_connect (const redisAsyncContext* parContext, int parStatus) {
		assert(parContext and parContext->data);
		Command& self = *static_cast<Command*>(parContext->data);

		self.m_connection_lost = false;
		self.m_connected = (parStatus == REDIS_OK);
	}

	void on_disconnect (const redisAsyncContext* parContext, int parStatus) {
		assert(parContext and parContext->data);
		Command& self = *static_cast<Command*>(parContext->data);
		assert(self.m_connected);

		self.m_connection_lost = (REDIS_ERR == parStatus);
		self.m_connected = false;
	};

	Command::Command (std::string&& parAddress, uint16_t parPort) :
		m_conn(nullptr, &redisAsyncDisconnect),
		m_address(std::move(parAddress)),
		m_port(parPort),
		m_connected(false),
		m_connection_lost(false)
	{
	}

	Command::Command (std::string&& parSocket) :
		Command(std::move(parSocket), 0)
	{
	}

	Command::~Command() noexcept {
	}

	void Command::connect() {
		if (not m_conn) {
			RedisConnection conn(
				(is_socket_connection() ?
					redisAsyncConnectUnix(m_address.c_str())
				:
					redisAsyncConnect(m_address.c_str(), m_port)
				),
				&redisAsyncDisconnect
			);
			if (not conn) {
				std::ostringstream oss;
				oss << "Unable to connect to Redis server at " << m_address << ':' << m_port;
				throw std::runtime_error(oss.str());
			}
			else {
				conn->data = this;
			}
			if (conn->err) {
				std::ostringstream oss;
				oss << "Unable to connect to Redis server at " << m_address << ':' << m_port <<
					": " << conn->errstr;
				throw std::runtime_error(oss.str());
			}
			if (REDIS_OK != redisAsyncSetConnectCallback(conn.get(), &on_connect))
				throw std::runtime_error("Unable to set \"on_connect()\" callback");
			if (REDIS_OK != redisAsyncSetDisconnectCallback(conn.get(), &on_disconnect))
				throw std::runtime_error("Unable to set \"on_disconnect()\" callback");
			std::swap(conn, m_conn);
		}
	}

	void Command::disconnect() {
		m_conn.reset();
	}

	bool Command::is_connected() const {
		const bool connected = m_conn and not m_conn->err and m_connected;
		assert(not m_connection_lost or connected);
		return connected;
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

#if defined(WITH_CRYPTOPP)
	boost::string_ref Command::add_lua_script_ifn (const std::string& parScript) {
		if (parScript.empty())
			return boost::string_ref();

		using dinhelp::lexical_cast;

		static_assert(20 == CryptoPP::SHA1::DIGESTSIZE, "Unexpected SHA1 digest size");
		static_assert(sizeof(LuaScriptHash) >= CryptoPP::SHA1::DIGESTSIZE, "Wrong SHA1 struct size");
		static_assert(Sha1Array().size() == CryptoPP::SHA1::DIGESTSIZE, "Wrong array size");

		LuaScriptHash digest;
		CryptoPP::SHA1().CalculateDigest(digest.raw_bytes, reinterpret_cast<const uint8_t*>(parScript.data()), parScript.size());
		//TODO: change when lexical_cast will support arrays
		const std::string sha1_str = lexical_cast<std::string>(digest.part_a) + lexical_cast<std::string>(digest.part_b) + lexical_cast<std::string>(digest.part_c);
		Sha1Array sha1_array;
		std::copy(sha1_str.begin(), sha1_str.end(), sha1_array.begin());

		auto it_found = m_known_hashes.find(sha1_array);
		const bool was_present = (m_known_hashes.end() != it_found);
		if (was_present) {
			return boost::string_ref(it_found->data(), it_found->size());
		}

		auto reply = this->run("SCRIPT", "LOAD", parScript);
		assert(not was_present);

		assert(get_string(reply) == sha1_str);
		const auto it_inserted = m_known_hashes.insert(it_found, sha1_array);
		(void)reply;

		return boost::string_ref(it_inserted->data(), it_inserted->size());
	}
#else
	boost::string_ref Command::add_lua_script_ifn (const std::string& parScript) {
		auto it_found = m_known_hashes.find(parScript);
		const bool was_present = (m_known_hashes.end() != it_found);
		if (was_present) {
			return boost::string_ref(it_found->second.data(), it_found->second.size());
		}

		auto reply = this->run("SCRIPT", "LOAD", parScript);
		assert(not was_present);

		const auto sha1_str = get_string(reply);
		Sha1Array sha1_array;
		std::copy(sha1_str.begin(), sha1_str.end(), sha1_array.begin());
		auto it_inserted = m_known_hashes.insert(it_found, std::make_pair(parScript, sha1_array));

		return boost::string_ref(it_inserted->second.data(), it_inserted->second.size());
	}
#endif

	bool Command::is_socket_connection() const {
		return not (m_port or m_address.empty());
	}
} //namespace redis
