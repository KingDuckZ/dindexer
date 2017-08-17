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

#include "dindexer-machinery/make_filerecord_tree.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include "pathname.hpp"
#include <algorithm>
#include <stack>

namespace mchlib {
	namespace {
		class Counter {
		public:
			Counter() : m_count(0) {}
			std::size_t operator()() {return m_count++;}
		private:
			std::size_t m_count;
		};
	} //unnamed namespace

	FileRecordNode::FileRecordNode() = default;
	FileRecordNode::~FileRecordNode() = default;

	std::vector<FileRecordNode> make_filerecord_tree (const std::vector<FileRecordData>& parRecords) {
		if (parRecords.empty())
			return std::vector<FileRecordNode>();

		std::vector<std::size_t> sorted_indices(parRecords.size());

		std::generate(sorted_indices.begin(), sorted_indices.end(), Counter());
		std::sort(sorted_indices.begin(), sorted_indices.end(), [&parRecords](std::size_t a, std::size_t b) {
			return parRecords[a].path() < parRecords[b].path();
		});

		FileRecordNode retval;
		FileRecordNode* curr_node = &retval;
		std::stack<FileRecordNode*> node_stack;

		assert(not sorted_indices.empty());
		PathName prev_dir("");

		for (auto idx : sorted_indices) {
			const FileRecordData& record = parRecords[idx];
			PathName curr_path(record.path());

			//1) new path is subpath
			if (is_ancestor(prev_dir, curr_path, 0)) {
				curr_node->children.emplace_back(idx);
			}
			//2) new path is different
			else {
				while (not node_stack.empty()) {
					curr_node = node_stack.top();
					node_stack.pop();

					PathName test_path(parRecords[curr_node->index].path());
					if (are_siblings(test_path, curr_path) or is_ancestor(test_path, curr_path, 0)) {
						prev_dir.swap(test_path);
						break;
					}
				};

				assert(curr_node);
				curr_node->children.emplace_back(idx);
			}

			if (record.is_directory) {
				node_stack.push(curr_node);
				assert(not curr_node->children.empty());
				curr_node = &curr_node->children.back();
				prev_dir.swap(curr_path);
			}
		}

		return retval.children;
	}
} //namespace mchlib
