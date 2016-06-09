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

#ifndef id9348909738B047B7B6912D73CB519039
#define id9348909738B047B7B6912D73CB519039

#include "helpers/compatibility.h"
#include <cstddef>
#include <boost/utility/string_ref.hpp>
#include <string>

namespace redis {
	namespace implem {
		template <typename T>
		const char* arg_to_bin_safe_char ( const T& parArg );

		template <typename T>
		std::size_t arg_to_bin_safe_length ( const T& parArg ) a_pure;

		template <typename T>
		struct MakeCharInfo;

		template<>
		struct MakeCharInfo<std::string> {
			MakeCharInfo ( const std::string& parData ) : m_string(parData) {}
			const char* data ( void ) const { return m_string.data(); }
			std::size_t size ( void ) const { return m_string.size(); }

		private:
			const std::string& m_string;
		};

		template<>
		struct MakeCharInfo<boost::string_ref> {
			MakeCharInfo ( const boost::string_ref& parData ) : m_data(parData.data()), m_size(parData.size()) {}
			const char* data ( void ) const { return m_data; }
			std::size_t size ( void ) const { return m_size; }

		private:
			const char* const m_data;
			const std::size_t m_size;
		};

		template <typename T>
		inline const char* arg_to_bin_safe_char (const T& parArg) {
			return MakeCharInfo<T>(parArg).data();
		}

		template <typename T>
		inline std::size_t arg_to_bin_safe_length (const T& parArg) {
			return MakeCharInfo<T>(parArg).size();
		}
	} //namespace implem
} //namespace redis

#endif
