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
#include "genericpath.hpp"
#include "dbsource.hpp"
#include "helpers/infix_iterator.hpp"
#include <cassert>
#include <utility>
#include <boost/range/algorithm/copy.hpp>
#include <sstream>
#include <ciso646>
#include <boost/lexical_cast.hpp>
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

	ListDirContent::ListDirContent (DBSource* parDB) :
		m_cache(g_max_cached_lists),
		m_db(parDB)
	{
		assert(m_db);
	}

	auto ListDirContent::ls (const GenericPath& parDir) const -> const ListType& {
		const std::string curr_path = parDir.to_string();
		{
			const auto* cached_item = find_and_refresh_in_cache(m_cache, curr_path);
			if (cached_item)
				return *cached_item;
		}

		//Requested item is not cached, so we need to query the db now
		if ("/" == curr_path) {
			auto sets_ids = m_db->sets();
			auto sets_info = m_db->set_details<SetDetail_ID, SetDetail_Desc, SetDetail_CreeationDate>(sets_ids);
			m_cache.push_back(std::make_pair(curr_path, db_result_to_vec(sets_info)));
		}
		else {
			const auto start_from = curr_path.find('/', 1);
			auto path_prefix = boost::string_ref(curr_path).substr(start_from == curr_path.npos ? curr_path.size() : start_from + 1);
			const auto set_id = boost::lexical_cast<uint32_t>(parDir[0]);
			auto files_info = m_db->file_details<FileDetail_Path>(set_id, parDir.level(), path_prefix);
			m_cache.push_back(std::make_pair(curr_path, db_result_to_vec(files_info)));
		}
		assert(not m_cache.empty());
		assert(m_cache.back().first == curr_path);
		return m_cache.back().second;
	}

	auto ListDirContent::ls ( GenericPath parDir, const std::string& parStartWith ) const -> const ListType& {
		parDir.push_piece(parStartWith);
		const std::string curr_path = parDir.to_string();

		{
			const auto* cached_item = find_and_refresh_in_cache(m_cache, curr_path);
			if (cached_item)
				return *cached_item;
		}

		if ("/" == curr_path) {
			assert(false); //not implemented
		}
		else {
			const auto start_from = curr_path.find('/', 1);
			auto path_prefix = boost::string_ref(curr_path).substr(start_from == curr_path.npos ? curr_path.size() : start_from + 1);
			const auto set_id = boost::lexical_cast<uint32_t>(parDir[0]);
			assert(parDir.level() > 0);
			auto file_list = m_db->paths_starting_by(set_id, parDir.level() - 1, path_prefix);
			m_cache.push_back(std::make_pair(curr_path, file_list));
		}
		assert(not m_cache.empty());
		assert(m_cache.back().first == curr_path);
		return m_cache.back().second;
	}
} //namespace din
