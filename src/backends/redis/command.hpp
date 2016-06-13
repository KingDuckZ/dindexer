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

#include "arg_to_bin_safe.hpp"
#include "scan_iterator.hpp"
#include "reply.hpp"
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

struct redisContext;

namespace redis {
	class Command {
	public:
		typedef ScanIterator<std::string, ScanSingleValues<std::string>> scan_iterator;
		typedef boost::iterator_range<scan_iterator> scan_range;
		typedef ScanIterator<std::pair<std::string, std::string>, ScanPairs<std::pair<std::string, std::string>>> hscan_iterator;
		typedef boost::iterator_range<hscan_iterator> hscan_range;

		Command ( std::string&& parAddress, uint16_t parPort );
		~Command ( void ) noexcept;

		void connect ( void );
		void disconnect ( void );

		bool is_connected ( void ) const;

		template <typename... Args>
		RedisReplyType run ( const char* parCommand, Args&&... parArgs );

		//Single Redis command wrappers
		scan_range scan ( void );
		hscan_range hscan ( boost::string_ref parKey );

	private:
		using RedisConnection = std::unique_ptr<redisContext, void(*)(redisContext*)>;

		RedisReplyType run_pvt ( int parArgc, const char** parArgv, std::size_t* parLengths );

		RedisConnection m_conn;
		std::string m_address;
		uint16_t m_port;
	};

	template <typename... Args>
	RedisReplyType Command::run (const char* parCommand, Args&&... parArgs) {
		constexpr const std::size_t arg_count = sizeof...(Args) + 1;
		using CharPointerArray = std::array<const char*, arg_count>;
		using LengthArray = std::array<std::size_t, arg_count>;
		using implem::arg_to_bin_safe_char;
		using implem::arg_to_bin_safe_length;
		using boost::string_ref;

		return this->run_pvt(
			static_cast<int>(arg_count),
			CharPointerArray{ arg_to_bin_safe_char(string_ref(parCommand)), arg_to_bin_safe_char(std::forward<Args>(parArgs))... }.data(),
			LengthArray{ arg_to_bin_safe_length(string_ref(parCommand)), arg_to_bin_safe_length(std::forward<Args>(parArgs))... }.data()
		);
	}
} //namespace redis

#endif
