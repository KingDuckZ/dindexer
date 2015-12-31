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

#ifndef idEC73C3E4D64D44ABA0DB7D41FA8A7EB7
#define idEC73C3E4D64D44ABA0DB7D41FA8A7EB7

#include <chrono>
#include <type_traits>
#include <boost/utility/string_ref.hpp>

namespace pq {
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

		template <> inline const char* type_to_pqtypes_name<std::string>() { return "%text"; }
		template <> inline const char* type_to_pqtypes_name<boost::string_ref>() { return "%text"; }
		template <> inline const char* type_to_pqtypes_name<bool>() { return "%bool"; }
		template <> inline const char* type_to_pqtypes_name<float>() { return "%float4"; }
		template <> inline const char* type_to_pqtypes_name<double>() { return "%float8"; }
		template <> inline const char* type_to_pqtypes_name<int16_t>() { return "%int2"; }
		template <> inline const char* type_to_pqtypes_name<int32_t>() { return "%int4"; }
		template <> inline const char* type_to_pqtypes_name<int64_t>() { return "%int8"; }
		template <> inline const char* type_to_pqtypes_name<uint16_t>() { return "%int2"; }
		template <> inline const char* type_to_pqtypes_name<uint32_t>() { return "%int4"; }
		template <> inline const char* type_to_pqtypes_name<uint64_t>() { return "%int8"; }
		template <> inline const char* type_to_pqtypes_name<std::chrono::system_clock::time_point>() { return "%timestamptz"; }
	} //namespace implem
} //namespace pq

#endif
