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

#include <array>
#include <memory>
#include <string>
#include <cstdint>
#include <cstddef>
#include <cassert>

struct redisContext;

namespace redis {
	class Command {
	public:
		Command ( std::string&& parAddress, uint16_t parPort, bool parConnect );
		~Command ( void ) noexcept;

		void connect ( void );
		void disconnect ( void );

		bool is_connected ( void ) const;

		template <typename... Args>
		void run ( const char* parCommand, Args&&... parArgs );

	private:
		using RedisConnection = std::unique_ptr<redisContext, void(*)(redisContext*)>;

		void run ( const char* parCommand, int parArgc, const char** parArgv, std::size_t* parLengths );

		RedisConnection m_conn;
		std::string m_address;
		uint16_t m_port;
	};

	template <typename... Args>
	void Command::run (const char* parCommand, Args&&... parArgs) {
		constexpr const std::size_t arg_count = sizeof...(Args);
		using CharPointerArray = std::array<const char*, arg_count>;
		using LengthArray = std::array<std::size_t, arg_count>;

		CharPointerArray arguments;
		LengthArray lengths;
		assert(false); //TODO write implementation

		this->run(
			parCommand,
			static_cast<int>(arg_count),
			arguments.data(),
			lengths.data()
		);
	}
} //namespace redis

#endif
