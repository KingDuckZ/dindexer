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

#ifndef id942A6B5AB2AF443C82D4321775BFC9E8
#define id942A6B5AB2AF443C82D4321775BFC9E8

#include "compatibility.h"
#include "helpers/sequence_bt.hpp"
#include <type_traits>
#include <utility>
#include <limits>
#include <math.h>
#include <array>
#include <cstdint>
#include <algorithm>
#include <string>
#include <ciso646>
#include <climits>
#include <boost/range/adaptor/reversed.hpp>

namespace dinhelp {
	namespace customize {
		template<typename T>
		struct index_array_to_string;

		template<typename C, typename T>
		struct char_to_int;
	} //namespace customize

	namespace implem {
		template <typename T>
		inline constexpr std::size_t max_digits() {
			return static_cast<std::size_t>(::log(std::numeric_limits<T>::max()));
		}

		template <std::size_t Base, std::size_t Val>
		struct power {
			enum { value = Base * power<Base, Val - 1>::value };
		};
		template <std::size_t Base>
		struct power<Base, 0> {
			enum { value = 1 };
		};

		constexpr std::size_t count_digits_10_compiletime (std::size_t parNum) {
			return (parNum == 0 ? 0 : static_cast<std::size_t>(::log10(parNum))) + 1;
		}

		template <std::size_t N>
		struct maxdigits {
			enum { value = count_digits_10_compiletime(static_cast<std::size_t>(::pow(2.0, static_cast<double>(N)))) };
		};

		template <template <typename> class Tag, typename T, typename F>
		inline std::array<uint8_t, max_digits<T>()> int_to_string (const F& parFrom) {
			using ArrayRetType = std::array<uint8_t, max_digits<T>()>;

			ArrayRetType retval;
			F div = 1;
			for (std::size_t z = 0; z < Tag<F>::count_digits(parFrom); ++z) {
				retval[Tag<F>::count_digits(parFrom) - z - 1] = static_cast<uint8_t>((parFrom / div) % Tag<F>::base);
				div *= Tag<F>::base;
			}
			std::fill(retval.begin() + Tag<F>::count_digits(parFrom), retval.end(), 0xff);
			return retval;
		};

		template <template <typename> class Tag, typename T, typename F>
		inline T string_to_int (const F& parFrom) {
			T retval(0);
			T mul(1);
			for (auto chara : boost::adaptors::reverse(parFrom)) {
				retval += dinhelp::customize::char_to_int<decltype(chara), T>::make(chara) * mul;
				mul *= Tag<T>::base;
			}
			return retval;
		};
	} //namespace implem

	namespace tags {
		template <typename T>
		struct dec {
			enum { base = 10 };

			template <T... Powers, std::size_t... Digits>
			static std::size_t count_digits (
				T parValue,
				dinhelp::bt::index_seq<Powers...> = dinhelp::bt::index_range<0, dinhelp::implem::max_digits<T>()>(),
				dinhelp::bt::index_seq<Digits...> = dinhelp::bt::index_range<0, CHAR_BIT * sizeof(T)>()
			) a_pure;
		};

		template <typename T>
		struct hex {
			enum { base = 16 };

			static std::size_t count_digits ( T parValue ) a_pure;
		};

		//See: http://stackoverflow.com/questions/9721042/count-number-of-digits-which-method-is-most-efficient#9721113
		template <typename T>
		template <T... Powers, std::size_t... Digits>
		std::size_t dec<T>::count_digits (T parValue, dinhelp::bt::index_seq<Powers...>, dinhelp::bt::index_seq<Digits...>) {
			static T powers[] = { 0, dinhelp::implem::power<10, Powers + 1>::value... };
			static std::size_t maxdigits[] = { dinhelp::implem::maxdigits<Digits>::value... };
			const auto bits = sizeof(parValue) * CHAR_BIT - __builtin_clz(parValue);
			return (parValue < powers[maxdigits[bits] - 1] ? maxdigits[bits] - 1 : maxdigits[bits]);
		}

		template <typename T>
		std::size_t hex<T>::count_digits (T parValue) {
			return std::max(((sizeof(parValue) * CHAR_BIT - __builtin_clz(parValue)) + (CHAR_BIT / 2 - 1)) / (CHAR_BIT / 2), 1);
		}
	} //namespace tags

	namespace implem {
		template <template <typename> class Tag>
		struct lexical_cast {
			template <typename T, typename F>
			static T convert ( const typename std::enable_if<std::is_integral<F>::value, F>::type& parFrom ) {
				const auto indices = int_to_string<Tag, T, F>(parFrom);
				return dinhelp::customize::index_array_to_string<T>::make(indices);
			}

			template <typename T, typename F>
			static typename std::enable_if<std::is_integral<T>::value, T>::type convert ( const F& parFrom ) {
				return string_to_int<Tag, T, F>(parFrom);
			}
		};
	} //namespace implem

	template <typename T, template <typename> class Tag=tags::dec, typename F=void>
	inline T lexical_cast (const F& parFrom) {
		return dinhelp::implem::lexical_cast<Tag>::template convert<T, F>(parFrom);
	}

	namespace customize {
		template<>
		struct index_array_to_string<std::string> {
			template<std::size_t N>
			static std::string make (const std::array<uint8_t, N> &parIndices) {
				static const char symbols[] = {'0', '1', '2', '3', '4', '5',
											   '6', '7', '8', '9', 'A', 'B',
											   'C', 'D', 'E', 'F'};
				std::string retval(N, ' ');
				for (std::size_t z = 0; z < N; ++z) {
					retval[z] = symbols[parIndices[z]];
				}
				return retval;
			}
		};

		template<typename T>
		struct char_to_int<char, T> {
			static T make (char parChar) {
				if (parChar >= '0' and parChar <= '9')
					return parChar - '0';
				else if (parChar >= 'a' and parChar <= 'f')
					return 10 + parChar - 'a';
				else if (parChar >= 'A' and parChar <= 'F')
					return 10 + parChar - 'A';
				return 0;
			}
		};
	} //namespace customize
} //namespace dinhelp
#endif
