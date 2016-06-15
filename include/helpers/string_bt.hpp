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

#ifndef id170B0E6C34D14EBA9B92A35977BDBFB3
#define id170B0E6C34D14EBA9B92A35977BDBFB3

#include "sequence_bt.hpp"
#include <cstddef>
#include <iostream>
#include <stdexcept>

namespace dinhelp {
	namespace bt {
		template <std::size_t S>
		class string;

		template <std::size_t S>
		std::ostream& operator<< ( std::ostream& parStream, const string<S>& parString );

		template <std::size_t S>
		class string {
		public:
			friend std::ostream& operator<< <>( std::ostream& parStream, const string<S>& parString );
			constexpr string ( const char* parString );

			constexpr std::size_t size ( void ) const { return S - 1; }
			template <std::size_t S2>
			constexpr string<S + S2 - 1> operator+ ( const string<S2>& parOther ) const;
			constexpr char operator[] ( std::size_t parIndex ) const;

			template <typename... Args>
			constexpr string ( Args... );

			constexpr const char (&data_arr() const)[S] { return m_data; }
			constexpr const char* data() const { return m_data; }

		private:
			template <std::size_t... I>
			constexpr string ( const index_seq<I...>&, const char* parString );

			const char m_data[S];
		};

		namespace implem {
			template <std::size_t S, std::size_t S2, std::size_t... I>
			constexpr string<S + S2 - 1> concat ( const index_seq<I...>&, const string<S>& parLeft, const string<S2>& parRight ) {
				return string<S + S2 - 1>(
					(I < S - 1 ? parLeft[I] : (I < S + S2 - 2 ? parRight[I - (S - 1)] : '\0'))...
				);
			}

		} //namespace implem

		template <std::size_t S>
		template <std::size_t... I>
		constexpr string<S>::string (const index_seq<I...>&, const char* parString) :
			m_data{parString[I]...}
		{
		}

		template <std::size_t S>
		inline constexpr string<S>::string (const char* parString) :
			string(index_range<0, S>(), parString)
		{
		}

		template <std::size_t S>
		template <typename... Args>
		inline constexpr string<S>::string (Args... parArgs) :
			m_data{parArgs...}
		{
		}

		template <std::size_t S>
		template <std::size_t S2>
		constexpr inline string<S + S2 - 1> string<S>::operator+ (const string<S2>& parOther) const {
			return implem::concat(index_range<0, S + S2 - 1>(), string<S>(m_data), parOther);
		}

		template <std::size_t S>
		inline std::ostream& operator<< (std::ostream& parStream, const string<S>& parString) {
			parStream << parString.m_data;
			return parStream;
		}

		template <std::size_t S>
		constexpr char string<S>::operator[] (std::size_t parIndex) const {
			return (parIndex < S ? m_data[parIndex] : throw std::out_of_range(""));
		}

		template <std::size_t S>
		constexpr string<S> make_string (const char (&parData)[S]) {
			return string<S>(parData);
		}
	} //namespace bt
} //namespace dinhelp

#endif
