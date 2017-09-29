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

#include "hash.hpp"
#include "dindexer-machinery/scantask/hashing.hpp"
#include "dindexer-machinery/scantask/singlefile.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include "dindexer-machinery/make_filerecord_tree.hpp"
#include <memory>
#include <sys/stat.h>
#include <stdexcept>
#include <utility>
#include <cassert>
#include <ciso646>

namespace stask = mchlib::scantask;

namespace din {
	namespace {
		void fill_hash_nodes (
			const std::vector<mchlib::FileRecordData>& parRefData,
			const std::vector<mchlib::FileRecordNode>& parNodesIn,
			std::vector<din::HashNode>& parNodesOut
		) {
			using std::string;

			const std::size_t sz = parNodesIn.size();
			parNodesOut.reserve(sz);
			for (const auto& in : parNodesIn) {
				assert(in.index < parRefData.size());
				const auto& data = parRefData[in.index];
				parNodesOut.push_back(HashNode{string(data.path()), data.hash, {}});
			}

			assert(parNodesOut.size() == sz);
			assert(parNodesIn.size() == sz);
			for (std::size_t z = 0; z < sz; ++z) {
				fill_hash_nodes(parRefData, parNodesIn[z].children, parNodesOut[z].children);
			}
		}
	} //unnamed namespace

	std::vector<HashNode> hash (const std::string& parPath) {
		using mchlib::FileRecordData;
		using HashingTaskPtr = std::shared_ptr<stask::Hashing>;
		using stask::SingleFileTask;

		struct stat path_stat;
		{
			const int retval = stat(parPath.c_str(), &path_stat);
			if (retval) {
				throw std::runtime_error("Can't access file \"" + parPath + "\"");
			}
		}

		std::shared_ptr<stask::Base<std::vector<FileRecordData>>> file_src_task;
		if (S_ISDIR(path_stat.st_mode)) {
			file_src_task.reset(new stask::DirTree(parPath));
		}
		else {
			assert(S_ISREG(path_stat.st_mode));
			file_src_task.reset(new SingleFileTask(parPath, &path_stat));
		}

		auto hashing = HashingTaskPtr(new stask::Hashing(file_src_task, false));
		const std::vector<FileRecordData>& hashes = hashing->get_or_create();

		std::vector<HashNode> retval;
		{
			std::vector<mchlib::FileRecordNode> tree_indices = mchlib::make_filerecord_tree(hashes);
			fill_hash_nodes(hashes, tree_indices, retval);
		}
		return retval;
	}
} //namespace din
