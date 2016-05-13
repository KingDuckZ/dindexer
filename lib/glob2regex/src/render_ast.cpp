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

#include "render_ast.hpp"
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <sstream>

namespace ka = boost::spirit::karma;

namespace g2r {
	namespace {
		template <typename Iterator>
		struct RegexGen : ka::grammar<Iterator, GlobExpression()> {
			RegexGen ( void );

			boost::spirit::karma::rule<Iterator, GlobExpression()> start;
			boost::spirit::karma::rule<Iterator, GlobAlternation()> alternation;
			boost::spirit::karma::rule<Iterator, GlobGroup()> group;
			boost::spirit::karma::rule<Iterator, std::string()> literal;
			boost::spirit::karma::rule<Iterator, GlobJolly()> jolly;
			boost::spirit::karma::symbols<bool, const char*> negate_group;
			boost::spirit::karma::symbols<char, const char*> special_char;
			boost::spirit::karma::symbols<GlobJolly, const char*> jolly_dic;
		};

		template <typename Iterator>
		RegexGen<Iterator>::RegexGen() :
			RegexGen<Iterator>::base_type(start)
		{
			using ka::lit;
			using ka::char_;
			using ka::eps;
			using ka::symbols;

			negate_group.add(true, "^")(false, "");
			special_char.add('.', "\\.")('*', "\\*")('\\', "\\\\")('?', "\\?")
				('(', "\\(")(')', "\\)")('[', "\\[")(']', "\\]")('{', "\\{")
				('}', "\\}")('$', "\\$")('^', "\\^")('+', "\\+")('|', "\\|");
			jolly_dic.add(GlobJolly(0, 0, false), "[^/]*")
				(GlobJolly(1, 1, false), "[^/]")
				(GlobJolly(0, 0, true), ".*");

			start = *(jolly | alternation | group | literal);
			group = "[" << negate_group << *(special_char | char_) << "]";
			alternation = eps << "(?:" << (start % '|') << ")";
			literal = *(special_char | char_);
			jolly = eps << jolly_dic;
		}
	} //unnamed namespace

	std::string render_ast (const GlobExpression& parAst) {
		RegexGen<boost::spirit::ostream_iterator> gramm;
		std::ostringstream oss;
		oss << ka::format(gramm, parAst);
		oss << '$';
		return oss.str();
	}
} //namespace g2r
