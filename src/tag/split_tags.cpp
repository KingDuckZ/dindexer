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

#include "split_tags.hpp"
#include <boost/algorithm/string/finder.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/utility/string_ref.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <ciso646>

namespace din {
	std::vector<boost::string_ref> split_tags (const std::string& parCommaSeparatedList) {
		using OutRange = boost::iterator_range<std::string::const_iterator>;
		using boost::token_finder;
		using boost::adaptors::transformed;
		using boost::adaptors::filtered;
		using boost::string_ref;
		using boost::iter_split;
		using boost::trim_copy;

		std::vector<OutRange> out_range;

		//See:
		//https://stackoverflow.com/questions/27999941/how-to-use-boostsplit-with-booststring-ref-in-boost-1-55
		//http://www.boost.org/doc/libs/1_60_0/doc/html/boost/algorithm/iter_split.html
		//http://www.boost.org/doc/libs/1_60_0/doc/html/boost/algorithm/token_finder.html
		//https://stackoverflow.com/questions/20781090/difference-between-boostsplit-vs-boostiter-split
		return boost::copy_range<std::vector<string_ref>>(
			iter_split(out_range, parCommaSeparatedList, token_finder([](char c){return ','==c;})) |
				transformed([](const OutRange& r){return trim_copy(r);}) |
				transformed([](const OutRange& r){return string_ref(&*r.begin(), r.size());}) |
				filtered([](const string_ref& r){return not r.empty();})
		);
	}
} //namespace din
