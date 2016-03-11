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

#ifndef id0AA31B2E7D6244A08435CF9080E34AAE
#define id0AA31B2E7D6244A08435CF9080E34AAE

#include "dindexer-machinery/scantask/base.hpp"
#include <string>
#include <vector>

namespace mchlib {
	struct FileRecordData;

	namespace scantask {
		class DirTree : public Base<std::vector<FileRecordData>> {
		public:
			typedef std::vector<FileRecordData> PathList;

			explicit DirTree ( std::string parRoot );
			virtual ~DirTree ( void ) noexcept = default;

		private:
			virtual void on_data_destroy ( PathList& parData ) override;
			virtual void on_data_create ( PathList& parData ) override;

			std::string m_root;
		};
	} //namespace scantask
} //namespace mchlib

#endif
