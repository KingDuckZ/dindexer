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

#ifndef idD83EEBFC927840C6B9F32D61A1D1E582
#define idD83EEBFC927840C6B9F32D61A1D1E582

#include "scan_iterator.hpp"
#include "reply.hpp"
#include "batch.hpp"
#include "redisConfig.h"
#include <array>
#include <memory>
#include <string>
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <vector>
#include <utility>
#include <boost/range/iterator_range_core.hpp>
#include <boost/utility/string_ref.hpp>
#include <stdexcept>
#if defined(WITH_CRYPTOPP)
#	include <set>
#else
#	include <map>
#endif
#include <boost/utility/string_ref.hpp>

struct redisAsyncContext;

namespace redis {
	class Command {
	public:
		typedef ScanIterator<ScanSingleValues<std::string>> scan_iterator;
		typedef boost::iterator_range<scan_iterator> scan_range;
		typedef ScanIterator<ScanPairs<std::pair<std::string, std::string>, ScanCommands::HSCAN>> hscan_iterator;
		typedef boost::iterator_range<hscan_iterator> hscan_range;
		typedef ScanIterator<ScanSingleValuesInKey<std::string>> sscan_iterator;
		typedef boost::iterator_range<sscan_iterator> sscan_range;
		typedef ScanIterator<ScanPairs<std::pair<std::string, std::string>, ScanCommands::ZSCAN>> zscan_iterator;
		typedef boost::iterator_range<zscan_iterator> zscan_range;

		Command ( std::string&& parAddress, uint16_t parPort );
		explicit Command ( std::string&& parSocket );
		~Command ( void ) noexcept;

		void connect ( void );
		void disconnect ( void );

		bool is_connected ( void ) const;

		Batch make_batch ( void );

		template <typename... Args>
		Reply run ( const char* parCommand, Args&&... parArgs );

		//Single Redis command wrappers
		scan_range scan ( void );
		hscan_range hscan ( boost::string_ref parKey );
		sscan_range sscan ( boost::string_ref parKey );
		zscan_range zscan ( boost::string_ref parKey );

		void submit_lua_script ( const std::string& parScript );

	private:
		using RedisConnection = std::unique_ptr<redisAsyncContext, void(*)(redisAsyncContext*)>;
		using Sha1Array = std::array<char, 20>;

		boost::string_ref add_lua_script_ifn ( const std::string& parScript );
		bool is_socket_connection ( void ) const;

		RedisConnection m_conn;
#if defined(WITH_CRYPTOPP)
		std::set<Sha1Array> m_known_hashes;
#else
		std::map<std::string, Sha1Array> m_known_scripts;
#endif
		std::string m_address;
		uint16_t m_port;
	};

	template <typename... Args>
	Reply Command::run (const char* parCommand, Args&&... parArgs) {
		auto batch = make_batch();
		batch.run(parCommand, std::forward<Args>(parArgs)...);
		batch.throw_if_failed();
		return batch.replies().front();
	}
} //namespace redis

#endif
