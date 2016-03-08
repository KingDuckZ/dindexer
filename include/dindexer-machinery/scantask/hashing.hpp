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

#ifndef idC7CC55298AC049EAA80604D6C7FD081D
#define idC7CC55298AC049EAA80604D6C7FD081D

#include "dindexer-machinery/scantask/leanbase.hpp"
#include "dindexer-machinery/tiger.hpp"
#include <vector>
#include <memory>

namespace mchlib {
	struct FileRecordData;

	namespace scantask {
		class Hashing : public LeanBase<std::vector<FileRecordData>> {
		public:
			typedef LeanBase<std::vector<FileRecordData>> FileTreeBase;

			Hashing ( std::shared_ptr<FileTreeBase> parFileTree, bool parIgnoreErrors );
			virtual ~Hashing ( void ) noexcept;

		private:
			virtual void on_data_fill ( void ) override;
			virtual std::vector<FileRecordData>& on_data_get ( void ) override;

			std::shared_ptr<FileTreeBase> m_file_tree_task;
			bool m_ignore_errors;
		};
	} //namespace scantask
} //namespace mchlib

#endif
