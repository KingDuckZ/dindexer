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

#include "searcher.hpp"
#include "glob2regex/glob2regex.hpp"
#include "backends/db_backend.hpp"
#include <iostream>
#include <string>
#include <algorithm>

namespace dindb {
	std::ostream &
	operator<<(std::ostream &parStream, const LocatedItem &parItem) {
		parStream << parItem.group_id << '\t' << parItem.id << '\t'
		          << parItem.path;
		return parStream;
	}
} //namespace dindb

namespace din {
	Searcher::Searcher (dindb::Backend& parBackend) :
		m_backend(parBackend)
	{
	}

	void Searcher::on_locate (const QString& parSearch) const {
		const auto search = parSearch.toStdString();
		const auto regex_str = g2r::convert(search, false);
#if !defined(NDEBUG)
		std::cout << "Searcher::on_locate() called with \"" << search << "\" --> \"" << regex_str << "\"\n";
#endif

		const auto results = m_backend.locate_in_db(regex_str, dindb::TagList());
		std::copy(results.begin(), results.end(), std::ostream_iterator<dindb::LocatedItem>(std::cout, "\n"));
	}
} //namespace din
