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

#ifndef id74048277752B4B85935C50F117F65DBE
#define id74048277752B4B85935C50F117F65DBE

#include <boost/fusion/adapted/struct.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <vector>
#include <string>
#include <ciso646>
#include <cstdint>

namespace g2r {
	struct GlobAlternation;

	struct GlobGroup {
		bool negated;
		std::string characters;
	};
	struct GlobJolly {
		GlobJolly ( void ) = default;
		constexpr GlobJolly ( const GlobJolly& ) = default;
		constexpr GlobJolly ( uint16_t m, uint16_t M, bool s) :
			match_length_min(m),
			match_length_max(M),
			match_slash(s)
		{
		}
		constexpr bool operator< ( GlobJolly o ) const {
			return
				// slash = 0, o.slash = 1
				(not match_slash and o.match_slash) or
				// slash = o.slash
				((not (match_slash xor o.match_slash)) and
				// max < o.max
				(((match_length_max < o.match_length_max) or
				// max == o.max
				(not (match_length_max xor o.match_length_max) and
				// min < o.min
				match_length_min < o.match_length_min))));
		}

		uint16_t match_length_min;
		uint16_t match_length_max;
		bool match_slash;
	};
} //namespace g2r

BOOST_FUSION_ADAPT_STRUCT(
	g2r::GlobGroup,
	(bool, negated)
	(std::string, characters)
);
BOOST_FUSION_ADAPT_STRUCT(
	g2r::GlobJolly,
	(uint16_t, match_length_min)
	(uint16_t, match_length_max)
	(bool, match_slash)
);

namespace g2r {
	using GlobNode = boost::variant<
		boost::recursive_wrapper<GlobAlternation>,
		GlobGroup,
		std::string,
		GlobJolly
	>;

	struct GlobAlternation {
		std::vector<std::vector<GlobNode>> alternatives;
	};

	using AstType = std::vector<GlobNode>;
} //namespace g2r

BOOST_FUSION_ADAPT_STRUCT(
	g2r::GlobAlternation,
	(std::vector<std::vector<g2r::GlobNode>>, alternatives)
);

static_assert(g2r::GlobJolly(1, 2, false) < g2r::GlobJolly(0, 0, true), "Wrong less than");
static_assert(not (g2r::GlobJolly(1, 2, false) < g2r::GlobJolly(1, 2, false)), "Wrong less than");
static_assert(g2r::GlobJolly(10, 2, false) < g2r::GlobJolly(0, 3, false), "Wrong less than");
static_assert(g2r::GlobJolly(10, 2, true) < g2r::GlobJolly(0, 3, true), "Wrong less than");
static_assert(not (g2r::GlobJolly(10, 3, false) < g2r::GlobJolly(0, 2, false)), "Wrong less than");
static_assert(not (g2r::GlobJolly(10, 3, true) < g2r::GlobJolly(0, 2, true)), "Wrong less than");
static_assert(g2r::GlobJolly(10, 3, false) < g2r::GlobJolly(11, 3, false), "Wrong less than");
static_assert(g2r::GlobJolly(10, 3, true) < g2r::GlobJolly(11, 3, true), "Wrong less than");

#endif
