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
		get_pqlib_c_type_struct_arr::get_pqlib_c_type_struct_arr (const Connection& parConn) :
			m_par(parConn.make_empty_params())
		{
			static_assert(sizeof(storage) == sizeof(PGarray), "Wrong size for PGarray's static memory");
			static_assert(alignof(storage) == alignof(PGarray), "Wrong alignment for PGarray's static memory");

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
