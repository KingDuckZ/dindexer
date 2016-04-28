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

#include "entrypath.hpp"
#include "helpers/infix_iterator.hpp"
#include <boost/spirit/include/qi_core.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <sstream>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/operator.hpp>
#include <cassert>

namespace qi = boost::spirit::qi;

namespace din {
	namespace {
		typedef std::vector<std::string> StringList;

		std::string hex_to_str (unsigned int parChara) {
			const char curr = static_cast<char>(parChara);
			return std::string(&curr, 1);
		}

		template <typename Iterator>
		struct PathGrammar : public qi::grammar<Iterator, StringList()> {
			PathGrammar ( void );

			qi::rule<Iterator, StringList()> start;
			qi::rule<Iterator, std::string()> diritem;
			qi::rule<Iterator, std::string()> escape;
			qi::rule<Iterator, std::string()> hex_code;
			qi::uint_parser<unsigned int, 16, 2, 2> hex_chara;
		};

		template <typename Iterator>
		PathGrammar<Iterator>::PathGrammar() :
			PathGrammar<Iterator>::base_type(start)
		{
			namespace px = boost::phoenix;
			using boost::spirit::qi::lit;
			using boost::spirit::qi::char_;
			using boost::spirit::qi::_val;
			using boost::spirit::_1;

			start %= *lit("/") >> *(diritem >> *lit("/"));
			diritem %= +(escape | (char_ - '/' - '\\'));
			escape %= lit("\\") >> (hex_code | char_);
			hex_code = lit("x") >> hex_chara[_val += px::bind(&hex_to_str, _1)];
		}
	} //unnamed namespace

	void EntryPath::push_piece (const std::string& parPiece) {
		using boost::spirit::qi::parse;

		PathGrammar<std::string::const_iterator> gramm;
		auto piece_begin = parPiece.begin();
		const std::string::const_iterator piece_end = parPiece.end();
		StringList result;

		const bool parse_result = parse(
			piece_begin,
			piece_end,
			gramm,
			result
		);

		assert(parse_result);
		if (not parse_result) {
			return;
		}

		if (not parPiece.empty() and parPiece[0] == '/') {
			m_stack.clear();
		}

		for (auto&& itm : result) {
			if (itm == "..") {
				if (not m_stack.empty()) {
					m_stack.pop_back();
				}
			}
			else if (itm == ".") {
			}
			else {
				m_stack.push_back(std::move(itm));
			}
		}
	}

	std::string EntryPath::to_string() const {
		std::ostringstream oss;
		oss << '/';
		boost::copy(m_stack, infix_ostream_iterator<std::string>(oss, "/"));
		return oss.str();
	}

	uint16_t EntryPath::level() const {
		return static_cast<uint16_t>(m_stack.size());
	}

	const std::string& EntryPath::operator[] (std::size_t parIndex) const {
		assert(parIndex < level());
		return m_stack[parIndex];
	}
} //namespace din
