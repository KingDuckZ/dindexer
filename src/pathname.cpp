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

#include "pathname.hpp"
#include <algorithm>
#include <functional>
#include <ciso646>

#include <iostream>

namespace din {
	namespace {
		bool ptr_between (const char* parPtr, const char* parBeg, const char* parEnd) {
			std::less<const char*> less;
			std::less_equal<const char*> lesseq;

			return lesseq(parBeg, parPtr) and less(parPtr, parEnd);
		}

		void split_path (std::vector<boost::string_ref>* parOut, boost::string_ref parPath) {
			parOut->clear();
			auto from = parPath.begin();
			boost::string_ref::const_iterator next;
			const auto end = parPath.end();
			const auto beg = parPath.begin();
			while (end != (next = std::find(from, end, '/'))) {
				if (next != from) {
					parOut->push_back(parPath.substr(from - beg, next - from));
					from = next;
				}
				++from;
			}
			if (next != from) {
				parOut->push_back(parPath.substr(from - beg, next - from));
			}
		}
	} //unnamed namespace

	PathName::PathName (const char* parPath) :
		m_original_path(parPath)
	{
		if (not m_original_path.empty()) {
			m_absolute = ('/' == *parPath);
			split_path(&m_atoms, m_original_path);
		}
	}

	std::string PathName::path() const {
		if (m_atoms.empty()) {
			if (m_absolute) {
				return std::string("/");
			}
			else {
				return std::string("");
			}
		}

		std::size_t reserve = (m_absolute ? 1 : 0);
		for (const auto& itm : m_atoms) {
			reserve += itm.size();
		}
		reserve += m_atoms.size() - 1;

		std::string out;
		out.reserve(reserve);
		const char* slash = (m_absolute ? "/" : "");
		for (const auto& itm : m_atoms) {
			out += slash;
			out.insert(out.end(), itm.begin(), itm.end());
			slash = "/";
		}
		return std::move(out);
	}

	void PathName::join (const PathName& parOther) {
		typedef std::pair<std::string, std::size_t> PairType;
		using boost::string_ref;

		for (const auto& itm : parOther.m_pool) {
			m_pool[itm.first] += itm.second;
		}
		const auto& other_path = parOther.original_path();
		const auto it_other_path = m_pool.insert(PairType(other_path, 0)).first;

		for (auto str : parOther.m_atoms) {
			if (ptr_between(str.data(), other_path.data(), other_path.data() + other_path.size())) {
				it_other_path->second++;
				auto offset = str.data() - other_path.data();
				m_atoms.push_back(string_ref(it_other_path->first).substr(offset, str.size()));
			}
			else {
			}
		}
		if (not it_other_path->second) {
			m_pool.erase(it_other_path);
		}

		std::cout << "    --------------- content -----------------\n";
		for (const auto& itm : m_pool) {
			std::cout << itm.first << " - " << itm.second << '\n';
		}
	}
} //namespace din
