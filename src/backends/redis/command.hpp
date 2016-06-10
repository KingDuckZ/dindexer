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
#include <array>
#include <memory>
#include <string>
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <vector>
#include <utility>

struct redisContext;

namespace redis {
	class RedisReplyType;

	namespace implem {
		using RedisVariantType = boost::variant<
			long long,
			std::string,
			boost::recursive_wrapper<std::vector<RedisReplyType>>
		>;
		enum RedisVariantTypes {
			RedisVariantType_Integer = 0,
			RedisVariantType_String,
			RedisVariantType_Array
		};
	} //namespace implem

	struct RedisReplyType : implem::RedisVariantType {
		using base_class = implem::RedisVariantType;

		RedisReplyType ( void ) = default;
		RedisReplyType ( long long parVal ) : base_class(parVal) {}
		RedisReplyType ( std::string&& parVal ) : base_class(std::move(parVal)) {}
		RedisReplyType ( std::vector<RedisReplyType>&& parVal ) : base_class(std::move(parVal)) {}
		~RedisReplyType ( void ) noexcept = default;
	};

	class Command {
	public:
		Command ( std::string&& parAddress, uint16_t parPort );
		~Command ( void ) noexcept;

		void connect ( void );
		void disconnect ( void );

		bool is_connected ( void ) const;

		template <typename... Args>
		RedisReplyType run ( const char* parCommand, Args&&... parArgs );

	private:
		using RedisConnection = std::unique_ptr<redisContext, void(*)(redisContext*)>;

		RedisReplyType run_pvt ( const char* parCommand, int parArgc, const char** parArgv, std::size_t* parLengths );

		RedisConnection m_conn;
		std::string m_address;
		uint16_t m_port;
	};

	template <typename... Args>
	RedisReplyType Command::run (const char* parCommand, Args&&... parArgs) {
		constexpr const std::size_t arg_count = sizeof...(Args);
		using CharPointerArray = std::array<const char*, arg_count>;
		using LengthArray = std::array<std::size_t, arg_count>;

		CharPointerArray arguments;
		LengthArray lengths;
		assert(false); //TODO write implementation

		return this->run_pvt(
			parCommand,
			static_cast<int>(arg_count),
			CharPointerArray{ implem::arg_to_bin_safe_char(std::forward<Args>(parArgs))... }.data(),
			LengthArray{ implem::arg_to_bin_safe_length(std::forward<Args>(parArgs))... }.data()
		);
	}

	long long get_integer ( const RedisReplyType& parReply );
	std::string get_string ( const RedisReplyType& parReply );
	std::vector<RedisReplyType> get_array ( const RedisReplyType& parReply );
} //namespace redis

#endif
