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

#ifndef idE173D2BA33744F448B870BB53AF52610
#define idE173D2BA33744F448B870BB53AF52610

#include "dindexer-machinery/scantask/base.hpp"
#include "dindexer-machinery/scantask/leanbase.hpp"
#include <vector>
#include <memory>

namespace mchlib {
	struct FileRecordData;

	namespace scantask {
		class Mime : public LeanBase<std::vector<FileRecordData>> {
		public:
			using DirTreeTaskPtr = std::shared_ptr<Base<std::vector<FileRecordData>>>;

			explicit Mime ( DirTreeTaskPtr parDirTree );
			virtual ~Mime ( void ) noexcept;

		private:
			virtual void on_data_fill ( void ) override;
			virtual std::vector<FileRecordData>& on_data_get ( void ) override;

			DirTreeTaskPtr m_file_tree_task;
		};
	} //namespace scantask
} //namespace mchlib

#endif
