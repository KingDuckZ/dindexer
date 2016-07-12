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

#ifndef id7D338900114548A890B1EECE0C4D3C4C
#define id7D338900114548A890B1EECE0C4D3C4C

#include "command.hpp"
#include <boost/optional.hpp>
#include <string>
#include <boost/utility/string_ref.hpp>
#include <vector>

namespace redis {
	class IncRedis {
	public:
		typedef boost::optional<std::string> opt_string;
		typedef boost::optional<std::vector<opt_string>> opt_string_list;

		IncRedis ( std::string&& parAddress, uint16_t parPort );
		explicit IncRedis ( std::string&& parSocket );
		~IncRedis ( void ) noexcept = default;

		void connect ( void );
		void wait_for_connect ( void );
		void disconnect ( void );
		void wait_for_disconnect ( void );
		bool is_connected ( void ) const { return m_command.is_connected(); }

		Command& command ( void ) { return m_command; }
		const Command& command ( void ) const { return m_command; }

		//Hash
		opt_string hget ( boost::string_ref parKey, boost::string_ref parField );
		int hincrby ( boost::string_ref parKey, boost::string_ref parField, int parInc );

		//Set
		opt_string_list srandmember ( boost::string_ref parKey, int parCount );
		opt_string srandmember ( boost::string_ref parKey );

	private:
		Command m_command;
	};
} //namespace redis

#endif
