/* Copyright 2015, Michele Santullo
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

#include "indexer.hpp"
#include "pathname.hpp"
#include <algorithm>

#if !defined(NDEBUG)
#	include <iostream>
#endif

namespace din {
	bool Indexer::add_path (const char* parPath, int parLevel, bool parIsDir, bool) {
		if (parLevel > 0) {
			PathLists& path_lists = (parIsDir ? m_directories : m_files);
			const auto size = static_cast<std::size_t>(parLevel);

			if (size > path_lists.size()) {
				path_lists.resize(size);
			}

			std::string path(parPath);
			auto insert_point = std::lower_bound(path_lists[size - 1].begin(), path_lists[size - 1].end(), path);
			path_lists[size - 1].insert(insert_point, std::move(path));
		} else {
			m_base_path = parPath;
		}
		return true;
	}

#if !defined(NDEBUG)
	void Indexer::dump() const {
		PathName base_path(m_base_path);

		std::cout << "---------------- FILE LIST ----------------\n";
		for (const auto& cur_list : m_files) {
			for (const auto& cur_itm : cur_list) {
				PathName cur_path(cur_itm);
				std::cout << make_relative_path(base_path, cur_path).path() << '\n';
			}
		}
		std::cout << "---------------- DIRECTORY LIST ----------------\n";
		for (const auto& cur_list : m_directories) {
			for (const auto& cur_itm : cur_list) {
				PathName cur_path(cur_itm);
				std::cout << make_relative_path(base_path, cur_path).path() << '\n';
			}
		}
	}
#endif
} //namespace din
