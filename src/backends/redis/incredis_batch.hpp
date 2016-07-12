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

#ifndef id3C772A92AB0E440DA84DAFD807BC962D
#define id3C772A92AB0E440DA84DAFD807BC962D

#include "batch.hpp"
#include <boost/utility/string_ref.hpp>

namespace redis {
	class IncRedisBatch {
	public:
		IncRedisBatch ( void ) = delete;
		IncRedisBatch ( IncRedisBatch&& ) = default;
		IncRedisBatch ( const Batch& ) = delete;
		IncRedisBatch ( Batch&& parBatch );

		void reset ( void );
		void throw_if_failed ( void );
		const std::vector<Reply>& replies ( void ) { return m_batch.replies(); }
		Batch& batch ( void ) { return m_batch; }
		const Batch& batch ( void ) const { return m_batch; }

		//Misc
		IncRedisBatch& select ( int parIndex );
		IncRedisBatch& client_setname ( boost::string_ref parName );
		template <typename... Args>
		IncRedisBatch& del ( Args&&... parArgs );

		//Hash
		IncRedisBatch& hget ( boost::string_ref parKey, boost::string_ref parField );
		template <typename... Args>
		IncRedisBatch& hmget ( boost::string_ref parKey, Args&&... parArgs );
		template <typename... Args>
		IncRedisBatch& hmset ( boost::string_ref parKey, Args&&... parArgs );
		IncRedisBatch& hincrby ( boost::string_ref parKey, boost::string_ref parField, int parInc );

		//Set
		IncRedisBatch& srandmember ( boost::string_ref parKey, int parCount );
		IncRedisBatch& srandmember ( boost::string_ref parKey );
		template <typename... Args>
		IncRedisBatch& sadd ( boost::string_ref parKey, Args&&... parArgs );

		//Script
		IncRedisBatch& script_flush ( void );

	private:
		Batch m_batch;
	};

	template <typename... Args>
	IncRedisBatch& IncRedisBatch::hmget (boost::string_ref parKey, Args&&... parArgs) {
		static_assert(sizeof...(Args) > 0, "No fields specified");
		m_batch.run("HMGET", parKey, std::forward<Args>(parArgs)...);
		return *this;
	}

	template <typename... Args>
	IncRedisBatch& IncRedisBatch::hmset (boost::string_ref parKey, Args&&... parArgs) {
		static_assert(sizeof...(Args) % 2 == 0, "Uneven number of parameters received");
		m_batch.run("HMSET", parKey, std::forward<Args>(parArgs)...);
		return *this;
	}

	template <typename... Args>
	IncRedisBatch& IncRedisBatch::sadd (boost::string_ref parKey, Args&&... parArgs) {
		static_assert(sizeof...(Args) > 0, "No members specified");
		m_batch.run("SADD", parKey, std::forward<Args>(parArgs)...);
		return *this;
	}

	template <typename... Args>
	IncRedisBatch& IncRedisBatch::del (Args&&... parArgs) {
		static_assert(sizeof...(Args) > 0, "No keys specified");
		m_batch.run("DEL", std::forward<Args>(parArgs)...);
		return *this;
	}
} //namespace redis

#endif
