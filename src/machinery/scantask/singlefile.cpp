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

#include "dindexer-machinery/scantask/singlefile.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include <cassert>
#include <ciso646>
#include <sys/stat.h>

namespace mchlib {
	namespace scantask {
		SingleFileTask::SingleFileTask (std::string parPath) :
			SingleFileTask(parPath, nullptr)
		{
		}

		SingleFileTask::SingleFileTask (std::string parPath, const struct stat* parStat) :
			m_path(std::move(parPath)),
			m_stat(parStat)
		{
			assert(not m_path.empty());
		}

		void SingleFileTask::on_data_destroy (PathList& parData) {
			assert(not parData.empty());
			parData.clear();
		}

		void SingleFileTask::on_data_create (PathList& parData) {
			struct stat path_stat;
			const struct stat* stat_to_use = m_stat;

			if (not stat_to_use) {
				const int retval = stat(m_path.c_str(), &path_stat);
				if (retval) {
					throw std::runtime_error("Can't access file \"" + m_path + "\"");
				}
				stat_to_use = &path_stat;
			}

			assert(parData.empty());
			parData.reserve(1);
			parData.push_back(mchlib::FileRecordData(
				std::string(m_path),
				0,
				stat_to_use->st_atime,
				stat_to_use->st_mtime,
				0,
				false,
				false
			));
		}
	} //namespace scantask
} //namespace mchlib
