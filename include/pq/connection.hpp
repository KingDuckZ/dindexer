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
#include "pq/implem/pq_type_helpers.hpp"
#include <string>
#include <cstdint>
#include <memory>
#include <boost/utility/string_ref.hpp>

struct pg_param;
typedef pg_param PGparam;

namespace pq {
	class Connection {
	public:
		Connection ( std::string&& parUsername, std::string&& parPasswd, std::string&& parDatabase, std::string&& parAddress, uint16_t parPort );
		Connection ( Connection&& );
		Connection ( const Connection& ) = delete;
		~Connection ( void ) noexcept;

		std::string error_message ( void ) const;
		bool is_connected ( void ) const noexcept;
		void connect ( void );
		void disconnect ( void );
		ResultSet query ( const std::string& parQuery );

		std::string escaped_literal ( const std::string& parString );
		std::string escaped_literal ( boost::string_ref parString );

		template <typename... Args>
		ResultSet query ( const std::string& parQuery, Args&&... parArgs );

		Connection& operator= ( Connection&& );
		Connection& operator= ( const Connection& ) = delete;

	private:
		struct LocalData;
		using PGParams = std::unique_ptr<::PGparam, void(*)(::PGparam*)>;

		ResultSet query_params ( const std::string& parQuery, PGParams& parParams );
		PGParams make_params ( const std::string* parTypes, ... );

		std::unique_ptr<LocalData> m_localData;
		std::string m_username;
		std::string m_passwd;
		std::string m_database;
		std::string m_address;
		uint16_t m_port;
	};

	template <typename... Args>
	ResultSet Connection::query (const std::string& parQuery, Args&&... parArgs) {
		using std::remove_cv;
		using std::remove_reference;

		auto make_pgparams = [&parArgs..., this](){
			using implem::make_pqtypes_name;
			using implem::concat_strings;

			auto types_bt = concat_strings(make_pqtypes_name<typename remove_cv<typename remove_reference<Args>::type>::type>()...);
			static_assert(types_bt.size() > 0, "Invalid empty types string (function called with no arguments?)");
			const std::string types(types_bt.data(), types_bt.size());
			return this->make_params(&types, implem::get_pqlib_c_type_struct<typename remove_cv<typename remove_reference<Args>::type>::type>().conv(parArgs)...);
		};
		PGParams pgparams = make_pgparams();

		return this->query_params(parQuery, pgparams);
	}
} //namespace pq

#endif
