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

#ifndef id5715FB806F4E4BD1880646F696A9B1F2
#define id5715FB806F4E4BD1880646F696A9B1F2

#include <vector>

namespace mchlib {
	struct FileRecordData;

	struct FileRecordNode {
		FileRecordNode();
		explicit FileRecordNode (std::size_t parIdx) : index(parIdx) {}
		~FileRecordNode();

		std::size_t index;
		std::vector<FileRecordNode> children;
		//FileRecordNode* parent;
	};

	std::vector<FileRecordNode> make_filerecord_tree (const std::vector<FileRecordData>& parRecords);
} //namespace mchlib

#endif
