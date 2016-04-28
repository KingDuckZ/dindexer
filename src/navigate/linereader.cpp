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

#include "linereader.hpp"
#include "listdircontent.hpp"
#include "dindexer-common/readline_wrapper.hpp"
#include "entrypath.hpp"
#include <cassert>
#include <ciso646>
#include <vector>
#include <functional>

namespace din {
	namespace {
		std::vector<std::string> list_matches (const ListDirContent& parLS, const std::string& parCurrPath, const std::string& parPrefix) {
			EntryPath full_prefix;
			if (not parCurrPath.empty()) {
				full_prefix.push_piece(parCurrPath);
			}
			if (not parPrefix.empty()) {
				return parLS.ls(full_prefix, parPrefix);
			}
			else {
				return parLS.ls(full_prefix);
			}
		}
	} //unnamed namespace

	LineReader::LineReader (const ListDirContent* parLS) :
		m_ls(parLS)
	{
		assert(m_ls);
	}

	std::string LineReader::read (const std::string& parMessage, const std::string& parCurrPath) {
		dinlib::ReadlineWrapper rl(std::bind(&list_matches, std::cref(*m_ls), std::cref(parCurrPath), std::placeholders::_1));

		return rl.read(parMessage);
	}
} //namespace din
