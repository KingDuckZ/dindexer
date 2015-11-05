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
#include <cstring>

namespace din {
	namespace {
		void split_path (std::vector<std::string>* parOut, const char* parPath, std::size_t parLen) {
			parOut->clear();
			const char* from = parPath;
			const char* next;
			while (parPath + parLen != (next = std::find(from, parPath + parLen, '/'))) {
				if (next != from) {
					parOut->push_back(std::string(from, next));
					from = next;
				}
				++from;
			}
			if (next != from) {
				parOut->push_back(std::string(from, next));
			}
		}
	} //unnamed namespace

	PathName::PathName (const char* parPath) {
		if (not parPath) {
			m_absolute = false;
		}
		else {
			m_absolute = ('/' == *parPath);
			split_path(&m_atoms, parPath, std::strlen(parPath));
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
			out += itm;
			slash = "/";
		}
		return std::move(out);
	}
} //namespace din
