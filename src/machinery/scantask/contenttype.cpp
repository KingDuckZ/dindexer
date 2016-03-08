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
#include <cassert>

namespace mchlib {
	namespace scantask {
		ContentType::ContentType (DirTreeTaskPtr parDirTree, MediaTypeTaskPtr parMediaType) :
			m_dir_tree(parDirTree),
			m_media_type(parMediaType)
		{
			assert(m_dir_tree);
			assert(m_media_type);
		}

		void ContentType::on_data_destroy (mchlib::ContentTypes& parData) {
			parData = ContentType_Unknown;
		}

		void ContentType::on_data_create (mchlib::ContentTypes& parData) {
			auto media_type = m_media_type->get_or_create();
			const auto& tree = m_dir_tree->get_or_create();
			parData = mchlib::guess_content_type(media_type, tree);
		}
	} //namespace scantask
} //namespace mchlib
