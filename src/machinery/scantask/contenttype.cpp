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

#include "dindexer-machinery/scantask/contenttype.hpp"
#include "dindexer-machinery/guess_content_type.hpp"
#include <cassert>

namespace mchlib {
	namespace scantask {
		ContentType::ContentType (SetTaskType parSet, DirTreeTaskPtr parDirTree, MediaTypeTaskPtr parMediaType) :
			m_set_task(parSet),
			m_dir_tree(parDirTree),
			m_media_type(parMediaType)
		{
			assert(m_set_task);
			assert(m_dir_tree);
			assert(m_media_type);
		}

		SetRecordDataFull& ContentType::on_data_get() {
			return m_set_task->get_or_create();
		}

		void ContentType::on_data_fill() {
			auto& data = m_set_task->get_or_create();
			auto media_type = char_to_media_type(m_media_type->get_or_create().type);
			const auto& tree = m_dir_tree->get_or_create();
			const auto cont_type = mchlib::guess_content_type(media_type, tree);
			data.content_type = content_type_to_char(cont_type);
		}
	} //namespace scantask
} //namespace mchlib
