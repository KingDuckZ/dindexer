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
#include <string>

struct stat;

namespace mchlib {
	namespace scantask {
		class SingleFileTask : public Base<std::vector<mchlib::FileRecordData>> {
		public:
			typedef std::vector<mchlib::FileRecordData> PathList;

			explicit SingleFileTask ( std::string parPath );
			explicit SingleFileTask ( std::string parPath, const struct stat* parStat );
			virtual ~SingleFileTask ( void ) noexcept = default;

		private:
			virtual void on_data_destroy ( PathList& parData ) override;
			virtual void on_data_create ( PathList& parData ) override;

			std::string m_path;
			const struct stat* m_stat;
		};
	} //namespace scantask
} //namespace mchlib
