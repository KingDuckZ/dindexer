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

#include "listdircontent.hpp"
#include "entrypath.hpp"
#include "db/dbsource.hpp"
#include "helpers/infix_iterator.hpp"
#include <cassert>
#include <utility>
#include <boost/range/algorithm/copy.hpp>
#include <sstream>
#include <ciso646>
#include <algorithm>

namespace din {
	namespace {
		template <typename V> std::vector<std::string> db_result_to_vec ( const V& parResult );
		const std::size_t g_max_cached_lists = 4;

		template <typename V>
		std::vector<std::string> db_result_to_vec (const V& parResult) {
			std::vector<std::string> result;
			result.reserve(parResult.size());

			for (const auto& row : parResult) {
				std::ostringstream oss;
				boost::copy(row, infix_ostream_iterator<std::string>(oss, "\t"));
				result.push_back(oss.str());
			}
			return result;
		}

		std::vector<std::string>* find_and_refresh_in_cache (boost::circular_buffer<std::pair<std::string, std::vector<std::string>>>& parCache, const std::string& parCurrPath) {
			using CachedItemType = std::pair<std::string, std::vector<std::string>>;

			//Check if requested item is already cached
			auto it_found = std::find_if(parCache.begin(), parCache.end(), [&parCurrPath](const CachedItemType& itm) { return itm.first == parCurrPath; });
			if (it_found != parCache.end()) {
				CachedItemType tmp;
				std::swap(tmp, *it_found);
				assert(it_found->first.empty());
				assert(it_found->second.empty());
				assert(tmp.first == parCurrPath);
				parCache.erase(it_found);
				assert(parCache.size() < g_max_cached_lists);
				parCache.push_back(std::move(tmp));
				assert(not parCache.empty());
				assert(parCache.back().first == parCurrPath);
				return &parCache.back().second;
			}
			else {
				return nullptr;
			}
		}
	} //unnamed namespace

	ListDirContent::ListDirContent (dindb::DBSource* parDB) :
		m_cache(g_max_cached_lists),
		m_db(parDB)
	{
		assert(m_db);
	}

	auto ListDirContent::ls (const EntryPath& parDir) const -> const ListType& {
		const std::string curr_path = parDir.to_string();
		{
			const auto* cached_item = find_and_refresh_in_cache(m_cache, curr_path);
			if (cached_item)
				return *cached_item;
		}

		//Requested item is not cached, so we need to query the db now
		if (parDir.points_to_group()) {
			auto sets_ids = m_db->sets();
			auto sets_info = m_db->set_details<dindb::SetDetail_ID, dindb::SetDetail_Desc, dindb::SetDetail_CreeationDate>(sets_ids);
			m_cache.push_back(std::make_pair(curr_path, db_result_to_vec(sets_info)));
		}
		else {
			auto path_prefix = parDir.file_path();
			const auto set_id = parDir.group_id();
			auto files_info = m_db->file_details<dindb::FileDetail_Path>(set_id, parDir.level() + 1, path_prefix);
			m_cache.push_back(std::make_pair(curr_path, db_result_to_vec(files_info)));
		}
		return last_cached_item(curr_path);
	}

	auto ListDirContent::ls ( EntryPath parDir, const std::string& parStartWith ) const -> const ListType& {
		const auto return_level = parDir.level();
		parDir.push_piece(parStartWith);
		const std::string curr_path = parDir.to_string();

		{
			const auto* cached_item = find_and_refresh_in_cache(m_cache, curr_path);
			if (cached_item)
				return *cached_item;
		}

		if (parDir.points_to_group()) {
			assert(false); //not implemented
		}
		else {
			const auto set_id = parDir.group_id();
			const auto path_prefix = parDir.file_path();
			auto file_list = m_db->paths_starting_by(set_id, parDir.level(), path_prefix);
			for (auto& file_item : file_list) {
				file_item = EntryPath(file_item)[return_level];
			}
			m_cache.push_back(std::make_pair(curr_path, file_list));
		}
		return last_cached_item(curr_path);
	}

	auto ListDirContent::last_cached_item (const std::string& parCurrPath) const -> const ListType& {
		assert(not m_cache.empty());
		assert(m_cache.back().first == parCurrPath);
#ifdef NDEBUG
		(void)parCurrPath;
#endif
		return m_cache.back().second;
	};
} //namespace din
