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

#include "dindexer-machinery/scantask/dirtree.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include "dindexer-machinery/set_listing.hpp"
#include "duckhandy/compatibility.h"
#include "filesearcher.hpp"
#include "pathname.hpp"
#include <utility>
#include <cassert>
#include <ciso646>
#include <functional>
#include <algorithm>

namespace mchlib {
	namespace {
		std::size_t calc_rel_path_offs ( const PathName& parRoot, boost::string_ref parPath ) a_pure;

		std::size_t calc_rel_path_offs (const PathName& parRoot, boost::string_ref parPath) {
			PathName path(parPath);
			PathName rel_path = make_relative_path(parRoot, path);
			const auto rel_path_len = rel_path.str_path_size();
			const auto path_len = path.str_path_size();
			assert(rel_path_len <= path_len);
			return path_len - rel_path_len;
		}

		bool add_path (scantask::DirTree::PathList& parOut, const PathName& parRoot, const char* parPath, const fastf::FileStats& parStats) {
			using boost::string_ref;

			auto it_before = SetListing::lower_bound(
				parOut,
				parPath,
				parStats.level,
				parStats.is_dir
			);

			//std::string curr_path(parPath);
			//const std::size_t offset = parBase.str_path_size() + 1;
			//for (FileRecordData& itm : parItems) {
			//	const auto curr_offset = std::min(parRelPathOffs, curr_path.size());
			//	itm.path = boost::string_ref(itm.abs_path).substr(curr_offset);
			//	assert(itm.path.data());
			//}

			parOut.insert(
				it_before,
				FileRecordData(
					parPath,
					calc_rel_path_offs(parRoot, string_ref(parPath)),
					parStats.atime,
					parStats.mtime,
					static_cast<uint16_t>(parStats.level),
					static_cast<bool>(parStats.is_dir),
					static_cast<bool>(parStats.is_symlink)
				)
			);
			return true;
		}
	}

	namespace scantask {
		DirTree::DirTree (std::string parRoot) :
			m_root(std::move(parRoot))
		{
			assert(not m_root.empty());
		}

		void DirTree::on_data_destroy (PathList& parData) {
			parData.clear();
		}

		void DirTree::on_data_create (PathList& parData) {
			using std::placeholders::_1;
			using std::placeholders::_2;
			using boost::string_ref;

			assert(parData.empty());

			fastf::FileSearcher searcher(m_root);
			fastf::FileSearcher::ConstCharVecType ext, ignore;

			searcher.SetFollowSymlinks(true);
			auto root_path = PathName(string_ref(m_root));
			searcher.SetCallback(
				fastf::FileSearcher::CallbackType(
					std::bind(&add_path, std::ref(parData), std::cref(root_path), _1, _2)
				)
			);
			searcher.Search(ext, ignore);
		}
	} //namespace scantask
} //namespace mchlib
