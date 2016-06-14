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

#ifndef id93FA96E3071745D9A1E45D4D29B9F7D0
#define id93FA96E3071745D9A1E45D4D29B9F7D0

#include <boost/variant/variant.hpp>
#include <string>
#include <vector>

namespace redis {
	struct Reply;

	namespace implem {
		using RedisVariantType = boost::variant<
			long long,
			std::string,
			std::vector<Reply>
		>;
		enum RedisVariantTypes {
			RedisVariantType_Integer = 0,
			RedisVariantType_String,
			RedisVariantType_Array,
			RedisVariantType_Bool
		};
	} //namespace implem

	struct Reply : implem::RedisVariantType {
		using base_class = implem::RedisVariantType;

		Reply ( void ) = default;
		Reply ( long long parVal ) : base_class(parVal) {}
		Reply ( std::string&& parVal ) : base_class(std::move(parVal)) {}
		Reply ( std::vector<Reply>&& parVal ) : base_class(std::move(parVal)) {}
		~Reply ( void ) noexcept = default;
	};

	const long long& get_integer ( const Reply& parReply );
	long long get_integer_autoconv_if_str ( const Reply& parReply );
	const std::string& get_string ( const Reply& parReply );
	const std::vector<Reply>& get_array ( const Reply& parReply );

	template <typename T>
	const T& get ( const Reply& parReply );
} //namespace redis

#endif
