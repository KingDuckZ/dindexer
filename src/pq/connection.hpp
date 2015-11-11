/* Copyright 2015, Michele Santullo
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

#ifndef id68BF7AAC5BD54AF2BBD6EC1318B6687E
#define id68BF7AAC5BD54AF2BBD6EC1318B6687E

#include "pq/resultset.hpp"
#include <string>
#include <cstdint>
#include <memory>

namespace pq {
	class Connection {
	public:
		Connection ( std::string&& parUsername, std::string&& parPasswd, std::string&& parDatabase, std::string&& parAddress, uint16_t parPort );
		~Connection ( void ) noexcept;

		std::string error_message ( void ) const;
		bool is_connected ( void ) const noexcept;
		void connect ( void );
		void disconnect ( void );
		void query_void ( const std::string& parQuery );
		ResultSet query ( const std::string& parQuery );

		std::string escape_literal ( const std::string& parString );

	private:
		struct LocalData;

		const std::string m_username;
		const std::string m_passwd;
		const std::string m_database;
		const std::string m_address;
		const uint16_t m_port;
		std::unique_ptr<LocalData> m_localData;
	};

} //namespace pq

#endif
