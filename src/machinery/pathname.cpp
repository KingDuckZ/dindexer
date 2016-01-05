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

namespace mchlib {
	const std::string PathName::m_empty_str("");

	namespace {
		std::string get_joint_atoms ( const StringPool<char>& parPool, bool parAbs, std::size_t parSkipRight=0 );

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

		std::string get_joint_atoms (const StringPool<char>& parPool, bool parAbs, std::size_t parSkipRight) {
			const auto orig_atom_count = parPool.size();
			const auto atom_count = (parSkipRight >= orig_atom_count ? 0 : orig_atom_count - parSkipRight);
			if (not atom_count) {
				if (parPool.empty() and parAbs) {
					return std::string("/");
				}
				else {
					return std::string("");
				}
			}

			std::size_t reserve = (parAbs ? 1 : 0);
			for (std::size_t z = 0; z < atom_count; ++z) {
				reserve += parPool[z].size();
			}
			reserve += atom_count - 1;

			std::string out;
			out.reserve(reserve);
			const char* slash = (parAbs ? "/" : "");
			for (std::size_t z = 0; z < atom_count; ++z) {
				out += slash;
				const auto& curr_itm = parPool[z];
				out.insert(out.end(), curr_itm.begin(), curr_itm.end());
				slash = "/";
			}
			return std::move(out);
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
		return get_joint_atoms(m_pool, m_absolute);
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

	PathName make_relative_path (const PathName& parBasePath, const PathName& parOtherPath) {
		if (not parBasePath.is_absolute() and parOtherPath.is_absolute()) {
			return parOtherPath;
		}

		std::size_t common_atoms = 0;
		{
			const std::size_t shortest = std::min(parOtherPath.atom_count(), parBasePath.atom_count());
			for (std::size_t z = 0; z < shortest; ++z) {
				if (parOtherPath[z] == parBasePath[z]) {
					++common_atoms;
				}
				else {
					break;
				}
			}
		}

		PathName retval("");
		const auto ellipses_count = parBasePath.atom_count() - common_atoms;
		for (std::size_t z = 0; z < ellipses_count; ++z) {
			retval.join("..");
		}

		const auto remaining_atoms = parOtherPath.atom_count() - common_atoms;
		for (std::size_t z = 0; z < remaining_atoms; ++z) {
			retval.join(parOtherPath[z + common_atoms], parOtherPath.get_stringref_source(z + common_atoms));
		}
		return std::move(retval);
	}

	const std::string* PathName::get_stringref_source (std::size_t parIndex) const {
		return m_pool.get_stringref_source(parIndex);
	}

	std::string PathName::dirname() const {
		if (this->atom_count() == 0)
			return std::string();

		return get_joint_atoms(m_pool, m_absolute, 1);
	}

	std::ostream& operator<< (std::ostream& parStream, const PathName& parPath) {
		parStream << parPath.path();
		return parStream;
	}

	PathName& PathName::pop_right() {
		m_pool.pop();
		return *this;
	}

	bool PathName::operator!= (const PathName& parOther) const {
		const auto count = atom_count();
		if (count != parOther.atom_count()) {
			return true;
		}

		for (std::size_t z = 0; z < count; ++z) {
			if ((*this)[z] != parOther[z]) {
				return true;
			}
		}
		return false;
	}

	bool PathName::operator== (const PathName& parOther) const {
		const auto count = atom_count();
		if (count != parOther.atom_count()) {
			return false;
		}

		for (std::size_t z = 0; z < count; ++z) {
			if ((*this)[z] != parOther[z]) {
				return false;
			}
		}
		return true;
	}
} //namespace mchlib
