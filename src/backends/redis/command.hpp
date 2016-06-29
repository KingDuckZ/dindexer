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

struct redisAsyncContext;
struct ev_loop;

namespace redis {
	class Command {
		friend void on_connect ( const redisAsyncContext*, int );
		friend void on_disconnect ( const redisAsyncContext*, int );
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
		void wait_for_connect ( void );
		void disconnect ( void );
		void wait_for_disconnect ( void );

		bool is_connected ( void ) const;
		boost::string_ref connection_error ( void ) const;

		Batch make_batch ( void );

		template <typename... Args>
		Reply run ( const char* parCommand, Args&&... parArgs );

		//Single Redis command wrappers
		scan_range scan ( void );
		hscan_range hscan ( boost::string_ref parKey );
		sscan_range sscan ( boost::string_ref parKey );
		zscan_range zscan ( boost::string_ref parKey );

		void submit_lua_script ( const std::string& parScript );
		void wakeup_thread();
		void lock();
		void unlock();

	private:
		using RedisConnection = std::unique_ptr<redisAsyncContext, void(*)(redisAsyncContext*)>;
		using LibevLoop = std::unique_ptr<ev_loop, void(*)(ev_loop*)>;

		bool is_socket_connection ( void ) const;
		void on_connect_successful ( void );

		struct LocalData;

		RedisConnection m_conn;
		LibevLoop m_libev_loop_thread;
		std::string m_address;
		std::unique_ptr<LocalData> m_local_data;
		uint16_t m_port;
		volatile bool m_connected;
		volatile bool m_connection_lost;
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
