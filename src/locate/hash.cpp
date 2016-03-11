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
#include "dindexer-machinery/scantask/dirtree.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include <memory>
#include <sys/stat.h>
#include <stdexcept>
#include <utility>
#include <cassert>
#include <ciso646>

namespace stask = mchlib::scantask;

namespace din {
	namespace {
		class SingleFileTask : public stask::Base<std::vector<mchlib::FileRecordData>> {
		public:
			typedef std::vector<mchlib::FileRecordData> PathList;

			SingleFileTask ( std::string parPath, const struct stat* parStat );
			virtual ~SingleFileTask ( void ) noexcept = default;

		private:
			virtual void on_data_destroy ( PathList& parData ) override;
			virtual void on_data_create ( PathList& parData ) override;

			std::string m_path;
			const struct stat* m_stat;
		};

		SingleFileTask::SingleFileTask (std::string parPath, const struct stat* parStat) :
			m_path(std::move(parPath)),
			m_stat(parStat)
		{
			assert(not m_path.empty());
			assert(m_stat);
		}

		void SingleFileTask::on_data_destroy (PathList& parData) {
			assert(not parData.empty());
			parData.clear();
		}

		void SingleFileTask::on_data_create (PathList& parData) {
			assert(parData.empty());
			parData.reserve(1);
			parData.push_back(mchlib::FileRecordData(
				std::string(m_path),
				0,
				m_stat->st_atime,
				m_stat->st_mtime,
				0,
				false,
				false
			));
		}
	} //unnamed namespace

	mchlib::TigerHash hash (const std::string& parPath) {
		using mchlib::FileRecordData;
		using HashingTaskPtr = std::shared_ptr<stask::Hashing>;

		struct stat path_stat;
		const int retval = stat(parPath.c_str(), &path_stat);
		if (retval) {
			throw std::runtime_error("Can't access file \"" + parPath + "\"");
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
		return hashing->get_or_create().front().hash;
	}
} //namespace din
