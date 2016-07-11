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

#ifndef idD81C81D99196491A8C9B68DED8ADD260
#define idD81C81D99196491A8C9B68DED8ADD260

#include "reply.hpp"
#include "arg_to_bin_safe.hpp"
#include <vector>
#include <memory>

namespace std {
	template <class R> class future;
	template <class T> struct atomic;
} //namespace std

namespace redis {
	class Command;
	class AsyncConnection;

	class Batch {
		friend class Command;
	public:
		Batch ( Batch&& parOther );
		Batch ( const Batch& ) = delete;
		~Batch ( void ) noexcept;

		const std::vector<Reply>& replies ( void );
		bool replies_requested ( void ) const;
		void throw_if_failed ( void );

		template <typename... Args>
		Batch& run ( const char* parCommand, Args&&... parArgs );

		template <typename... Args>
		Batch& operator() ( const char* parCommand, Args&&... parArgs );

		void reset ( void ) noexcept;

	private:
		struct LocalData;

		explicit Batch ( AsyncConnection* parConn );
		void run_pvt ( int parArgc, const char** parArgv, std::size_t* parLengths );

		std::vector<std::future<Reply>> m_futures;
		std::vector<Reply> m_replies;
		std::unique_ptr<LocalData> m_local_data;
		AsyncConnection* m_async_conn;
	};

	class RedisError : public std::runtime_error {
	public:
		RedisError ( const char* parMessage, std::size_t parLength );
	};

	template <typename... Args>
	Batch& Batch::run (const char* parCommand, Args&&... parArgs) {
		constexpr const std::size_t arg_count = sizeof...(Args) + 1;
		using CharPointerArray = std::array<const char*, arg_count>;
		using LengthArray = std::array<std::size_t, arg_count>;
		using implem::arg_to_bin_safe_char;
		using implem::arg_to_bin_safe_length;
		using implem::MakeCharInfo;
		using boost::string_ref;

		this->run_pvt(
			static_cast<int>(arg_count),
			CharPointerArray{ (arg_to_bin_safe_char(string_ref(parCommand))), MakeCharInfo<typename std::remove_const<typename std::remove_reference<Args>::type>::type>(std::forward<Args>(parArgs)).data()... }.data(),
			LengthArray{ arg_to_bin_safe_length(string_ref(parCommand)), arg_to_bin_safe_length(std::forward<Args>(parArgs))... }.data()
		);

		return *this;
	}

	template <typename... Args>
	Batch& Batch::operator() (const char* parCommand, Args&&... parArgs) {
		return this->run(parCommand, std::forward<Args>(parArgs)...);
	}
} //namespace redis

#endif
