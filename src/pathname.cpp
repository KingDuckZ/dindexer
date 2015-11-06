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

namespace din {
	const std::string PathName::m_empty_str("");

	namespace {
		bool ptr_between (const char* parPtr, const char* parBeg, const char* parEnd) {
			std::less<const char*> less;
			std::less_equal<const char*> lesseq;

			return lesseq(parBeg, parPtr) and less(parPtr, parEnd);
		}

		std::size_t count_grouped (boost::string_ref parIn, char parDelim) {
			std::size_t retval = 0;
			char prev = '\0';
			for (auto c : parIn) {
				retval += (parDelim == c and prev != parDelim ? 1 : 0);
				prev = c;
			}
			return retval;
		}

		void split_path (std::vector<boost::string_ref>* parOut, boost::string_ref parPath) {
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

	PathName::PathName (boost::string_ref parPath) {
		if (not parPath.empty()) {
			m_absolute = ('/' == parPath.front());
			std::string path(parPath.begin(), parPath.end());

			const auto count = count_grouped(path, '/');
			const std::size_t trailing = (path.back() == '/' ? 1 : 0);
			const std::size_t absolute = (m_absolute ? 1 : 0);
			const auto res = count + 1 - trailing - absolute;
			std::vector<boost::string_ref> atoms;
			atoms.reserve(res);
			split_path(&atoms, path);
			m_pool.insert(atoms, &path);
		}
		else {
			m_original_path = nullptr;
			m_absolute = false;
		}
	}

	std::string PathName::path() const {
		if (m_pool.empty()) {
			if (m_absolute) {
				return std::string("/");
			}
			else {
				return std::string("");
			}
		}

		std::size_t reserve = (m_absolute ? 1 : 0);
		for (const auto& itm : m_pool) {
			reserve += itm.size();
		}
		reserve += m_pool.size() - 1;

		std::string out;
		out.reserve(reserve);
		const char* slash = (m_absolute ? "/" : "");
		for (const auto& itm : m_pool) {
			out += slash;
			out.insert(out.end(), itm.begin(), itm.end());
			slash = "/";
		}
		return std::move(out);
	}

	void PathName::join (const PathName& parOther) {
		m_pool.update(parOther.m_pool);
	}

	const boost::string_ref PathName::operator[] (std::size_t parIndex) const {
		return *(m_pool.begin() + parIndex);
	}

	std::size_t PathName::atom_count ( void ) const {
		return m_pool.size();
	}

	void PathName::join (const char* parOther) {
		const std::string src(parOther);
		const boost::string_ref ref(src);
		m_pool.insert(ref, &src);
	}

	void PathName::join (boost::string_ref parOther, const std::string* parSource) {
		m_pool.insert(parOther, parSource);
	}
} //namespace din
