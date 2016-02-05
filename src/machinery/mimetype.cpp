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

#include "mimetype.hpp"
#include <magic.h>
#include <ciso646>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <boost/spirit/include/qi_core.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_raw.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/stl/container.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/operator.hpp>

namespace mchlib {
	using MagicCookie = std::unique_ptr<magic_set, void(*)(magic_t)>;

	struct MimeType::LocalData {
		LocalData ( void ) :
			magic_cookie(nullptr, &magic_close)
		{
		}

		MagicCookie magic_cookie;
	};

	MimeType::MimeType() :
		m_local_data(new LocalData)
	{
		auto raw_cookie = magic_open(MAGIC_SYMLINK | MAGIC_MIME | MAGIC_PRESERVE_ATIME | MAGIC_ERROR);
		MagicCookie cookie(raw_cookie, &magic_close);

		if (raw_cookie) {
			if (0 != magic_load(raw_cookie, nullptr)) {
				std::ostringstream oss;
				oss << "MimeType cannot load magic database: \"" <<
					magic_error(raw_cookie) <<
					"\"";

				throw std::runtime_error(oss.str());
			}
		}

		std::swap(m_local_data->magic_cookie, cookie);
	}

	MimeType::~MimeType() noexcept {
	}

	bool MimeType::initialized() const {
		return not not m_local_data->magic_cookie;
	}

	const char* MimeType::analyze (const std::string& parPath) {
		assert(not parPath.empty());
		assert(parPath.c_str());
		if (initialized()) {
			const auto cookie = m_local_data->magic_cookie.get();
			const auto retval = magic_file(cookie, parPath.c_str());
			if (not retval) {
				std::ostringstream oss;
				oss << "MimeType failed to analyze \"" <<
					parPath << "\": \"" << magic_error(cookie) << "\"";

				throw std::runtime_error(oss.str());
			}
			return retval;
		}
		else {
			return nullptr;
		}
	}

	SplitMime split_mime (const std::string& parFull) {
		using boost::spirit::ascii::space;
		using boost::spirit::qi::char_;
		using boost::spirit::qi::lit;
		using boost::spirit::qi::raw;
		using boost::spirit::qi::_val;
		using boost::string_ref;
		using boost::phoenix::construct;
		using boost::spirit::_1;
		using boost::phoenix::begin;
		using boost::phoenix::size;
		namespace px = boost::phoenix;
		using RuleType = boost::spirit::qi::rule<std::string::const_iterator, string_ref(), boost::spirit::ascii::space_type>;

		SplitMime retval;

		auto full_it = parFull.begin();
		const auto beg = parFull.begin();
		RuleType mime_token = raw[+(char_ - ';')][_val = px::bind(&string_ref::substr, construct<string_ref>(px::ref(parFull)), begin(_1) - px::ref(beg), size(_1))];
		RuleType charset_token = raw[+(char_)][_val = px::bind(&string_ref::substr, construct<string_ref>(px::ref(parFull)), begin(_1) - px::ref(beg), size(_1))];

		const bool parse_ret = boost::spirit::qi::phrase_parse(
			full_it,
			parFull.end(),
			mime_token >> ';' >> lit("charset=") >> charset_token,
			space,
			retval
		);

		if (not parse_ret or parFull.end() != full_it) {
			return SplitMime();
		}
		else {
			return retval;
		}
	}
} //namespace mchlib
