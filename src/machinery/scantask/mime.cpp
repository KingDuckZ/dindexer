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

#include "dindexer-machinery/scantask/mime.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include "mimetype.hpp"
#include <cassert>

namespace mchlib {
	namespace {
	} //unnamed namespace

	namespace scantask {
		Mime::Mime (DirTreeTaskPtr parDirTree, bool parIgnoreErrors) :
			m_file_tree_task(parDirTree),
			m_ignore_errors(parIgnoreErrors)
		{
			assert(m_file_tree_task);
		}

		Mime::~Mime() noexcept {
		}

		void Mime::on_data_fill() {
			MimeType mime;
			auto& list = m_file_tree_task->get_or_create();

			for (auto& itm : list) {
				try {
					itm.mime_full = mime.analyze(itm.abs_path);
					auto mime_pair = split_mime(itm.mime_full);
					itm.set_mime_parts(mime_pair.first, mime_pair.second);
				}
				catch (const std::runtime_error&) {
					itm.mime_full = "";
				}
			}
		}

		std::vector<FileRecordData>& Mime::on_data_get() {
			return m_file_tree_task->get_or_create();
		}
	} //namespace scantask
} //namespace mchlib
