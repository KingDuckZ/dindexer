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

#include "dindexer-machinery/scantask/mediatype.hpp"
//#include "dindexer-machinery/guess_content_type.hpp"
#if defined(WITH_MEDIA_AUTODETECT)
#	include "dindexer-machinery/mediatype.hpp"
#endif
#include "dindexer-machinery/recorddata.hpp"
#include <utility>

namespace mchlib {
	namespace scantask {
		MediaType::MediaType (SetTaskType parSet, char parDefault, bool parForce, std::string parSearchPath) :
			m_set_task(parSet),
			m_default(char_to_media_type(parDefault))
#if defined(WITH_MEDIA_AUTODETECT)
			, m_search_path(std::move(parSearchPath))
			, m_force(parForce)
#endif
		{
			assert(m_set_task);
#if !defined(WITH_MEDIA_AUTODETECT)
			static_cast<void>(parForce);
			static_cast<void>(parSearchPath);
#endif
		}

		SetRecordDataFull& MediaType::on_data_get() {
			return m_set_task->get_or_create();
		}

		void MediaType::on_data_fill() {
			auto& data = m_set_task->get_or_create();
#if defined(WITH_MEDIA_AUTODETECT)
			if (m_force) {
				data.type = media_type_to_char(m_default);
			}
			else {
				const auto guessed_type = mchlib::guess_media_type(std::string(m_search_path));
				data.type = media_type_to_char(guessed_type);
			}
#else
			data.type = media_type_to_char(m_default);
#endif
		}
	} //namespace scantask
} //namespace mchlib
