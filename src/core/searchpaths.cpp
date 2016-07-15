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

#include "searchpaths.hpp"
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <ciso646>
#include <cassert>

namespace dincore {
	namespace {
		const std::string& STR_to_string (const std::string& parString) {
			return parString;
		}

		std::string STR_to_string (boost::string_ref parString) {
			return std::string(parString.data(), parString.size());
		}

		template <typename STR>
		std::string first_hit_impl (const std::vector<STR>& parPaths, SearchPaths::Predicate parPredicate, SearchPaths::SearchType parType) {
			using boost::filesystem::path;
			using boost::filesystem::is_directory;
			using boost::filesystem::directory_iterator;
			using boost::filesystem::directory_entry;
			using boost::make_iterator_range;

			for (const auto& curr_dir_path : parPaths) {
				path curr_path(STR_to_string(curr_dir_path));
				auto listing = make_iterator_range(directory_iterator(curr_path), directory_iterator());
				for (const directory_entry& entry : listing) {
					if (
						(parType == SearchPaths::Any) or
						(parType == SearchPaths::Directory and is_directory(entry)) or
						(parType == SearchPaths::File and not is_directory(entry))
					) {
						auto str_path = entry.path().string();
						if (parPredicate(curr_dir_path, str_path))
							return str_path;
					}
				}
			}
			return std::string();
		}

		//std::string make_file_path (boost::string_ref parPath, boost::string_ref parName) {
		//	assert(not parName.empty());
		//	if (parName.empty())
		//		return std::string(parPath.data(), parPath.size());

		//	std::string retval;
		//	const std::size_t slash = (not parPath.empty() and parPath[parPath.size() - 1] != '/' ? 1 : 0);
		//	retval.reserve(parPath.size() + parName.size() + slash);
		//	std::copy(parPath.begin(), parPath.end(), retval.begin());
		//	retval[parPath.size()] = '/';
		//	std::copy(parName.begin(), parName.end(), retval.begin() + parPath.size() + slash);
		//	return retval;
		//}

		bool is_same_filename (boost::string_ref parBaseDir, const std::string& parFullPath, boost::string_ref parFilename) {
			assert(parBaseDir.size() < parFullPath.size());
			return boost::string_ref(parFullPath).substr(parBaseDir.size() + 1) == parFilename;
		}
	} //unnamed namespace

	SearchPaths::SearchPaths (std::vector<std::string>&& parList) :
		m_paths(std::move(parList))
	{
	}

	SearchPaths::SearchPaths (std::initializer_list<std::string> parInit) :
		m_paths(parInit)
	{
	}

	SearchPaths::~SearchPaths() noexcept = default;

	void SearchPaths::add_path (std::string&& parPath) {
		if (std::find(m_paths.begin(), m_paths.end(), parPath) == m_paths.end()) {
			m_paths.emplace_back(std::move(parPath));
		}
	}

	std::string SearchPaths::first_hit (boost::string_ref parFile, SearchType parType) const {
		using std::placeholders::_1;
		using std::placeholders::_2;
		return first_hit_impl(m_paths, std::bind(&is_same_filename, _1, _2, parFile), parType);
	}

	std::string SearchPaths::first_hit (Predicate parPredicate, SearchType parType) const {
		return first_hit_impl(m_paths, parPredicate, parType);
	}

	ShallowSearchPaths::ShallowSearchPaths (std::vector<boost::string_ref>&& parList) :
		m_paths(std::move(parList))
	{
	}

	ShallowSearchPaths::ShallowSearchPaths (std::initializer_list<boost::string_ref> parInit) :
		m_paths(parInit)
	{
	}

	ShallowSearchPaths::~ShallowSearchPaths() noexcept = default;

	void ShallowSearchPaths::add_path (boost::string_ref parPath) {
		if (std::find(m_paths.begin(), m_paths.end(), parPath) == m_paths.end()) {
			m_paths.push_back(parPath);
		}
	}

	std::string ShallowSearchPaths::first_hit (boost::string_ref parFile, SearchPaths::SearchType parType) const {
		using std::placeholders::_1;
		using std::placeholders::_2;
		return first_hit_impl(m_paths, std::bind(&is_same_filename, _1, _2, parFile), parType);
	}

	std::string ShallowSearchPaths::first_hit (SearchPaths::Predicate parPredicate, SearchPaths::SearchType parType) const {
		return first_hit_impl(m_paths, parPredicate, parType);
	}
} //namespace dincore
