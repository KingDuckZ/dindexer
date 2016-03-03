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
#include "filesearcher.hpp"
#include <utility>
#include <cassert>
#include <ciso646>
#include <functional>

namespace mchlib {
	namespace {
		FileRecordData make_file_record_data (const char* parPath, const fastf::FileStats& parSt) {
			return FileRecordData(
				parPath,
				parSt.atime,
				parSt.mtime,
				parSt.level,
				parSt.is_dir,
				parSt.is_symlink
			);
		}

		bool add_path (scantask::DirTree::PathList& parOut, const char* parPath, const fastf::FileStats& parStats) {
			auto it_before = SetListing::lower_bound(
				parOut,
				parPath,
				parStats.level,
				parStats.is_dir
			);

			parOut.insert(
				it_before,
				make_file_record_data(parPath, parStats)
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

			assert(parData.empty());

			fastf::FileSearcher searcher(m_root);
			fastf::FileSearcher::ConstCharVecType ext, ignore;

			searcher.SetFollowSymlinks(true);
			searcher.SetCallback(fastf::FileSearcher::CallbackType(std::bind(&add_path, std::ref(parData), _1, _2)));
			searcher.Search(ext, ignore);
		}
	} //namespace scantask
} //namespace mchlib
