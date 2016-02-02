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

#include "pq/implem/pq_type_helpers.hpp"
#include "pq/connection.hpp"
#include <algorithm>
#include <cstdarg>
#include "libpqtypes.h"
#include <cassert>
#if !defined(NDEBUG)
#	include <cstring>
#endif

namespace pq {
	namespace implem {
		template <
			typename Expected,
			typename Struct,
			std::size_t ES=sizeof(Expected),
			std::size_t EA=alignof(Expected),
			std::size_t SS=sizeof(Struct),
			std::size_t SA=alignof(Struct)
		>
		void static_assert_size() {
			//static_assert(sizeof(PGparam*) == sizeof(void*), "Unexpected pointer size");
			//static_assert(sizeof(PGresult*) == sizeof(void*), "Unexpected pointer size");
			static_assert(ES == SS, "Wrong size for static memory");
			static_assert(EA == SA, "Wrong alignment for static memory");
		}

		auto get_pqlib_c_type_struct<std::chrono::system_clock::time_point>::conv (const std::chrono::system_clock::time_point& parParam) -> type {
			static_assert(sizeof(storage) == sizeof(PGtimestamp), "Wrong size for timestamp, please update DATA_SIZE");
			static_assert(alignof(storage) == alignof(PGtimestamp), "Wrong alignment for timestamp, please update type");

			using std::chrono::system_clock;

			PGtimestamp ts;

			std::memset(&ts, 0, sizeof(PGtimestamp));

			auto t = system_clock::to_time_t(parParam);
			ts.epoch = t;
			auto tm = std::localtime(&t);
			ts.time.hour = tm->tm_hour;
			ts.time.min = tm->tm_min;
			ts.time.sec = tm->tm_sec;
			ts.time.usec = 0;
			ts.time.withtz = 1;
			ts.date.isbc = 0;
			ts.date.year = tm->tm_year + 1900;
			ts.date.mon = tm->tm_mon;
			ts.date.mday = tm->tm_mday;
			char* tzn;
			PQlocalTZInfo(&t, &ts.time.gmtoff, &ts.time.isdst, &tzn);
			std::strcpy(ts.time.tzabbr, tzn);

			std::copy(reinterpret_cast<const char*>(&ts), reinterpret_cast<const char*>(&ts) + sizeof(ts), reinterpret_cast<char*>(&m_storage));
			return &m_storage;
		}

		get_pqlib_c_type_struct<std::chrono::system_clock::time_point>::~get_pqlib_c_type_struct ( void ) noexcept {
			return;
		}

		get_pqlib_c_type_struct_arr::get_pqlib_c_type_struct_arr (const Connection& parConn) :
			m_par(parConn.make_empty_params())
		{
			static_assert_size<PGarray, storage>();

			PGarray arr;
			std::fill(reinterpret_cast<char*>(&arr), reinterpret_cast<char*>(&arr) + sizeof(PGarray), '\0');
			arr.param = m_par.get();

			std::copy(reinterpret_cast<const char*>(&arr), reinterpret_cast<const char*>(&arr) + sizeof(PGarray), reinterpret_cast<char*>(&m_storage));
			assert(not std::memcmp(&arr, &m_storage, sizeof(PGarray)));
			assert(not arr.ndims);
			assert(not arr.dims[0]);
			assert(arr.param);
		}

		get_pqlib_c_type_struct_arr::~get_pqlib_c_type_struct_arr() noexcept {
		}

		void get_pqlib_c_type_struct_arr::par_reset() {
			PQparamReset(m_par.get());
		}

		const void* get_pqlib_c_type_struct_arr::get_return_ptr() {
			assert(m_par);
			return reinterpret_cast<const void*>(&m_storage);
		}

		void get_pqlib_c_type_struct_arr::push_param (const char* parFormat, ...) {
			//Only one argument is expected, but type and size on the stack is unknown :/
			va_list argp;

			va_start(argp, parFormat);
			PQputvf(m_par.get(), nullptr, 0, parFormat, argp);
			va_end(argp);
		}
	} //namespace implem
} //namespace pq
