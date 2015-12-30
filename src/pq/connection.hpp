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
#include <boost/utility/string_ref.hpp>
#include <chrono>
#include <type_traits>

struct pg_param;
typedef pg_param PGparam;

namespace pq {
	class Connection {
	public:
		Connection ( std::string&& parUsername, std::string&& parPasswd, std::string&& parDatabase, std::string&& parAddress, uint16_t parPort );
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

	private:
		struct LocalData;
		using PGParams = std::unique_ptr<::PGparam, void(*)(::PGparam*)>;

		ResultSet query_params ( const std::string& parQuery, PGParams& parParams );
		PGParams make_params ( const std::string* parTypes, ... );

		const std::string m_username;
		const std::string m_passwd;
		const std::string m_database;
		const std::string m_address;
		const uint16_t m_port;
		std::unique_ptr<LocalData> m_localData;
	};

	namespace implem {
		template <typename T>
		const char* type_to_pqtypes_name ( void );

		template <typename T>
		struct get_pqlib_c_type_struct {
			using type = T;
			static type conv ( T parParam ) { return parParam; }
		};
		template <>
		struct get_pqlib_c_type_struct<std::string> {
			using type = const char*;
			static type conv ( const std::string& parParam ) { return parParam.c_str(); }
		};
		template <>
		struct get_pqlib_c_type_struct<boost::string_ref> {
			using type = const char*;
			static type conv ( const boost::string_ref& parParam ) { return parParam.data(); }
		};
		template <>
		struct get_pqlib_c_type_struct<bool> {
			using type = int;
			static type conv ( bool parParam ) { return (parParam ? 1 : 0); }
		};
		template <>
		struct get_pqlib_c_type_struct<std::chrono::system_clock::time_point> {
			struct StorageStruct { uint64_t epoch; int a[14]; char tzabbr[16]; };
			static constexpr std::size_t DATA_SIZE = sizeof(StorageStruct);
			using storage = std::aligned_storage<DATA_SIZE, alignof(uint64_t)>::type;
			storage m_storage;

		public:
			using type = const storage*;

			type conv ( const std::chrono::system_clock::time_point& parParam );
			~get_pqlib_c_type_struct ( void ) noexcept;
		};

		template <typename T>
		inline typename get_pqlib_c_type_struct<T>::type get_pqlib_c_type (const T& parParam) {
			return get_pqlib_c_type_struct<T>::conv(parParam);
		}
	} //namespace implem

	template <typename... Args>
	ResultSet Connection::query (const std::string& parQuery, Args&&... parArgs) {
		using std::remove_cv;
		using std::remove_reference;

		auto make_pgparams = [&parArgs..., this](){
			using implem::type_to_pqtypes_name;

			std::string types;
			int unpack[] {0, (types += type_to_pqtypes_name<typename remove_cv<typename remove_reference<Args>::type>::type>(), types += ' ', 0)...};
			if (not types.empty()) {
				types.resize(types.size() - 1);
			}
			static_cast<void>(unpack);

			return this->make_params(&types, implem::get_pqlib_c_type_struct<typename remove_cv<typename remove_reference<Args>::type>::type>().conv(parArgs)...);
		};
		PGParams pgparams = make_pgparams();

		return this->query_params(parQuery, pgparams);
	}
} //namespace pq

#endif
