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

#include "pq/implem/string_bt.hpp"
#include <chrono>
#include <type_traits>
#include <boost/utility/string_ref.hpp>

namespace pq {
	namespace implem {
		template <typename T>
		struct type_to_pqtypes_name;

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

		template <>
		struct type_to_pqtypes_name<std::string> {
			constexpr static const char* name = "text";
		};
		template <>
		struct type_to_pqtypes_name<boost::string_ref> {
			constexpr static const char* name = "text";
		};
		template <>
		struct type_to_pqtypes_name<bool> {
			constexpr static const char* name = "bool";
		};
		template <>
		struct type_to_pqtypes_name<float> {
			constexpr static const char* name = "float4";
		};
		template <>
		struct type_to_pqtypes_name<double> {
			constexpr static const char* name = "float8";
		};
		template <>
		struct type_to_pqtypes_name<int16_t> {
			constexpr static const char* name = "int2";
		};
		template <>
		struct type_to_pqtypes_name<int32_t> {
			constexpr static const char* name = "int4";
		};
		template <>
		struct type_to_pqtypes_name<int64_t> {
			constexpr static const char* name = "int8";
		};
		template <>
		struct type_to_pqtypes_name<uint16_t> {
			constexpr static const char* name = "int2";
		};
		template <>
		struct type_to_pqtypes_name<uint32_t> {
			constexpr static const char* name = "int4";
		};
		template <>
		struct type_to_pqtypes_name<uint64_t> {
			constexpr static const char* name = "int8";
		};
		template <>
		struct type_to_pqtypes_name<std::chrono::system_clock::time_point> {
			constexpr static const char* name = "timestamptz";
		};

		constexpr inline std::size_t strlen (const char* parStr) {
			return (*parStr ? 1 + strlen(parStr + 1) : 0);
		}

		template <typename T>
		struct type_to_pqtypes_name_impl : public type_to_pqtypes_name<T> {
			constexpr static const std::size_t size = strlen(type_to_pqtypes_name<T>::name) + 1;
		};

		template <typename T>
		constexpr bt::string<type_to_pqtypes_name_impl<T>::size + 1> make_pqtypes_name ( const char parPrefix='%' );

		template <typename T>
		inline constexpr
		bt::string<type_to_pqtypes_name_impl<T>::size + 1> make_pqtypes_name (const char parPrefix) {
			return bt::make_string<2>({parPrefix, '\0'}) + bt::string<type_to_pqtypes_name_impl<T>::size>(type_to_pqtypes_name_impl<T>::name);
		}

		template <std::size_t... Sizes>
		struct total_string_len;
		template <>
		struct total_string_len<> {
			constexpr static const std::size_t result = 1;
		};
		template <std::size_t S1, std::size_t... Sizes>
		struct total_string_len<S1, Sizes...> {
			constexpr static const std::size_t result = S1 - 1 + total_string_len<Sizes...>::result;
		};

		inline constexpr
		bt::string<1> spaced_concat_strings() {
			return bt::string<1>("");
		}
		template <std::size_t S1>
		inline constexpr
		bt::string<S1 + 1> spaced_concat_strings (bt::string<S1> parA1) {
			return bt::make_string(" ") + parA1;
		}
		template <std::size_t S1, std::size_t S2, std::size_t... Sizes>
		constexpr inline
		bt::string<total_string_len<S1, S2, Sizes...>::result + 2 + sizeof...(Sizes)> spaced_concat_strings (bt::string<S1> parA1, bt::string<S2> parA2, bt::string<Sizes>... parArgs) {
			return (
				bt::make_string(" ") + parA1 +
				bt::make_string(" ") + parA2 +
				spaced_concat_strings(parArgs...)
			);
		}

		inline constexpr
		bt::string<1> concat_strings() {
			return bt::string<1>("");
		}
		template <std::size_t S1>
		inline constexpr
		bt::string<S1> concat_strings (bt::string<S1> parA1) {
			return parA1;
		}
		template <std::size_t S1, std::size_t S2, std::size_t... Sizes>
		constexpr inline
		bt::string<total_string_len<S1, S2, Sizes...>::result + 1 + sizeof...(Sizes)> concat_strings (bt::string<S1> parA1, bt::string<S2> parA2, bt::string<Sizes>... parArgs) {
			return (
				parA1 +
				bt::make_string(" ") + parA2 +
				spaced_concat_strings(parArgs...)
			);
		}
	} //namespace implem
} //namespace pq

#endif
