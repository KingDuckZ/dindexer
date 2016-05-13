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

#include "glob_ast.hpp"
#include "glob2regex/exceptions.hpp"
#include <boost/spirit/include/qi.hpp>
//#include <boost/spirit/include/qi_core.hpp>
//#include <boost/spirit/include/qi_parse.hpp>
//#include <boost/spirit/include/qi_lit.hpp>
//#include <boost/spirit/include/qi_char_.hpp>
//#include <boost/spirit/include/qi_plus.hpp>
//#include <boost/spirit/include/qi_matches.hpp>
//#include <boost/spirit/include/qi_raw.hpp>
//#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/stl/container.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/operator.hpp>
#include <ciso646>

//#define AST_DEBUG_OUTPUT

#if defined(AST_DEBUG_OUTPUT)
#	include <iostream>
#endif

namespace qi = boost::spirit::qi;

namespace g2r {
	namespace {
		template <typename Iterator>
		struct GlobGrammar : qi::grammar<Iterator, GlobExpression()> {
			GlobGrammar ( void );
			~GlobGrammar ( void ) = default;

			qi::rule<Iterator, GlobExpression()> start;
			qi::rule<Iterator, GlobExpression()> alternation_list;
			qi::rule<Iterator, GlobAlternation()> alternation;
			qi::rule<Iterator, GlobGroup()> group;
			qi::rule<Iterator, std::string()> literal;
			qi::rule<Iterator, std::string()> single_char_comma_list;
			qi::rule<Iterator, char()> escaped_glob;
			qi::rule<Iterator, GlobJolly()> jolly;
		};

		template <typename Iterator>
		GlobGrammar<Iterator>::GlobGrammar() :
			GlobGrammar<Iterator>::base_type(start)
		{
			using boost::spirit::qi::lit;
			using boost::spirit::qi::char_;
			using boost::spirit::qi::matches;
			using boost::spirit::qi::eps;
			using boost::spirit::qi::string;
			using boost::spirit::qi::as_string;
			using boost::spirit::qi::attr;
			using boost::spirit::qi::repeat;
			using boost::spirit::qi::inf;

			static const char* const special_char_list = "{}[]*\\+? ";
			const uint16_t uint16_zero = 0;
			const uint16_t uint16_one = 1;

			start = *(group | alternation | literal | jolly);
			alternation_list = *(group | alternation | as_string[+(~char_("{}[]*\\+? ,") | escaped_glob)] | jolly);
			single_char_comma_list = ~char_(special_char_list) % ",";
			alternation = eps >> lit("{") >> (alternation_list % ",") >> "}";
			group =
				(lit("[") >> matches[lit("!")] >> as_string[-string("]") >> *(~char_(']') | escaped_glob)] >> "]") |
				(attr(false) >> lit("{") >> single_char_comma_list >> lit("}"));
			literal = +(~char_(special_char_list) | escaped_glob);
			escaped_glob = lit("\\") >> char_(special_char_list);
			jolly = (attr(uint16_zero) >> attr(uint16_zero) >> attr(true) >> "**") |
				(attr(uint16_zero) >> attr(uint16_zero) >> attr(false) >> "*") |
				(attr(uint16_one) >> attr(uint16_one) >> attr(false) >> "?")
			;
		}
	} //unnamed namespace

	GlobExpression make_ast (const std::string& parGlob) {
		GlobGrammar<std::string::const_iterator> gramm;

		auto glob_beg = parGlob.cbegin();
		GlobExpression glob_ast;
		const bool parse_ret = boost::spirit::qi::parse(
			glob_beg,
			parGlob.end(),
			gramm,
			glob_ast
		);

#if defined(AST_DEBUG_OUTPUT)
		std::cout << "make_ast(\"" << parGlob << "\") - parse_ret = ";
		if (parse_ret)
			std::cout << "true";
		else
			std::cout << "false";
		std::cout << ", glob_ast.size() = " << glob_ast.size() << std::endl;
#endif

		if (not parse_ret or parGlob.end() != glob_beg)
			throw ParsingError(parGlob, parGlob.end() - glob_beg);

		return glob_ast;
	}
} //namespace g2r
