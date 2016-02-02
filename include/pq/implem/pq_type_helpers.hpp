/* Copyright 2016, Michele Santullo
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
#include <vector>
#include <memory>
#include <cassert>

struct pg_param;
typedef pg_param PGparam;

namespace pq {
	class Connection;

	namespace implem {
		template <typename T>
		struct type_to_pqtypes_name;

		template <typename T>
		struct get_pqlib_c_type_struct {
			using type = T;
			explicit get_pqlib_c_type_struct ( const Connection& ) { }
			static type conv ( T parParam ) { return parParam; }
		};
		template <>
		struct get_pqlib_c_type_struct<std::string> {
			using type = const char*;
			explicit get_pqlib_c_type_struct ( const Connection& ) { }
			static type conv ( const std::string& parParam ) { return parParam.c_str(); }
		};
		template <>
		struct get_pqlib_c_type_struct<boost::string_ref> {
		private:
			std::string m_str;
		public:
			using type = const char*;
			explicit get_pqlib_c_type_struct ( const Connection& ) { }
			type conv ( boost::string_ref parParam ) { m_str = std::string(parParam.data(), parParam.size()); return m_str.c_str(); }
		};
		template <>
		struct get_pqlib_c_type_struct<bool> {
			using type = int;
			explicit get_pqlib_c_type_struct ( const Connection& ) { }
			static type conv ( bool parParam ) { return (parParam ? 1 : 0); }
		};
		template <>
		struct get_pqlib_c_type_struct<std::chrono::system_clock::time_point> {
			//Hack to make some sort of "static pimpl"
			//I don't want to include libpqtypes here since this is a public
			//header, so the following should mimic the real struct in that they
			//should have the same size and alignment. This is asserted in the
			//cpp.
			struct StorageStruct { uint64_t epoch; int a[14]; char tzabbr[16]; };
			static constexpr std::size_t DATA_SIZE = sizeof(StorageStruct);
			using storage = std::aligned_storage<DATA_SIZE, alignof(uint64_t)>::type;
			storage m_storage;

		public:
			using type = const storage*;

			explicit get_pqlib_c_type_struct ( const Connection& ) { }
			type conv ( const std::chrono::system_clock::time_point& parParam );
			~get_pqlib_c_type_struct ( void ) noexcept;
		};

		template <typename T>
		inline typename get_pqlib_c_type_struct<T>::type get_pqlib_c_type (const T& parParam) {
			return get_pqlib_c_type_struct<T>::conv(parParam);
		}

		constexpr inline std::size_t strlen (const char* parStr) {
			return (*parStr ? 1 + strlen(parStr + 1) : 0);
		}
		template <std::size_t S>
		constexpr inline std::size_t strlen (bt::string<S> parStr) {
			return parStr.size();
		}

		template <typename T>
		struct type_to_pqtypes_name_impl : public type_to_pqtypes_name<T> {
			constexpr static const std::size_t size = strlen(type_to_pqtypes_name<T>::name()) + 1;
		};

		template <>
		struct type_to_pqtypes_name<std::string> {
			constexpr static const char* name ( void ) { return "text*"; }
		};
		template <>
		struct type_to_pqtypes_name<boost::string_ref> {
			constexpr static const char* name ( void ) { return "text"; }
		};
		template <>
		struct type_to_pqtypes_name<bool> {
			constexpr static const char* name ( void ) { return "bool"; }
		};
		template <>
		struct type_to_pqtypes_name<float> {
			constexpr static const char* name ( void ) { return "float4"; }
		};
		template <>
		struct type_to_pqtypes_name<double> {
			constexpr static const char* name ( void ) { return "float8"; }
		};
		template <>
		struct type_to_pqtypes_name<int16_t> {
			constexpr static const char* name ( void ) { return "int2"; }
		};
		template <>
		struct type_to_pqtypes_name<int32_t> {
			constexpr static const char* name ( void ) { return "int4"; }
		};
		template <>
		struct type_to_pqtypes_name<int64_t> {
			constexpr static const char* name ( void ) { return "int8"; }
		};
		template <>
		struct type_to_pqtypes_name<uint16_t> {
			constexpr static const char* name ( void ) { return "int2"; }
		};
		template <>
		struct type_to_pqtypes_name<uint32_t> {
			constexpr static const char* name ( void ) { return "int4"; }
		};
		template <>
		struct type_to_pqtypes_name<uint64_t> {
			constexpr static const char* name ( void ) { return "int8"; }
		};
		template <>
		struct type_to_pqtypes_name<std::chrono::system_clock::time_point> {
			constexpr static const char* name ( void ) { return "timestamptz"; }
		};
		template <typename VT, typename VU>
		struct type_to_pqtypes_name<std::vector<VT, VU>> {
		private:
			constexpr static const std::size_t sub_size = strlen(type_to_pqtypes_name<VT>::name()) + 1;
		public:
			constexpr static bt::string<3 + sub_size - 1> name ( void ) { return bt::string<sub_size>(type_to_pqtypes_name<VT>::name()) + bt::make_string("[]"); }
		};

		template <typename T>
		constexpr bt::string<type_to_pqtypes_name_impl<T>::size + 1> make_pqtypes_name ( const char parPrefix='%' );

		//Helper class to build a parameter of array type
		struct get_pqlib_c_type_struct_arr {
		private:
			using PGParams = std::unique_ptr<::PGparam, void(*)(::PGparam*)>;

			//Hack to make some sort of "static pimpl"
			struct StorageStruct { int a; int b[2 * 6]; void* c[2]; };
			static constexpr std::size_t DATA_SIZE = sizeof(StorageStruct);
			using storage = std::aligned_storage<DATA_SIZE, alignof(int)>::type;
			void push_param ( const char* parFormat, ... );
			storage m_storage;
			PGParams m_par;

		protected:
			explicit get_pqlib_c_type_struct_arr ( const Connection& parConn );
			~get_pqlib_c_type_struct_arr ( void ) noexcept;
			void par_reset ( void );
			const void* get_return_ptr ( void );
			template <typename T>
			void push_param ( const T& parParam ) {
				static_assert(std::is_fundamental<T>::value or std::is_same<std::string, T>::value or std::is_same<boost::string_ref, T>::value or std::is_same<std::chrono::system_clock::time_point, T>::value, "Unsupported type in array");

				this->push_param(make_pqtypes_name<T>().data(), get_pqlib_c_type_struct<T>::conv(parParam));
			}
		};
		template <typename VT, typename VU>
		struct get_pqlib_c_type_struct<std::vector<VT, VU>> : private get_pqlib_c_type_struct_arr {
			using type = const void*;
			explicit get_pqlib_c_type_struct ( const Connection& parConn ) : get_pqlib_c_type_struct_arr(parConn) { }
			type conv ( const std::vector<VT, VU>& parParam) {
				this->par_reset();
				for (const auto& i : parParam) {
					this->push_param(i);
				}
				return get_return_ptr();
			}
		};

		template <typename T>
		inline constexpr
		bt::string<type_to_pqtypes_name_impl<T>::size + 1> make_pqtypes_name (const char parPrefix) {
			return bt::make_string<2>({parPrefix, '\0'}) + bt::string<type_to_pqtypes_name_impl<T>::size>(type_to_pqtypes_name_impl<T>::name());
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
