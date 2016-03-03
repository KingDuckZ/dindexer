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
#include <utility>

namespace mchlib {
	namespace scantask {
		MediaType::MediaType (char parDefault, bool parForce, std::string parSearchPath) :
			m_default(dinlib::char_to_media_type(parDefault))
#if defined(WITH_MEDIA_AUTODETECT)
			, m_search_path(std::move(parSearchPath))
			, m_force(parForce)
#endif
		{
#if !defined(WITH_MEDIA_AUTODETECT)
			static_cast<void>(parForce);
			static_cast<void>(parSearchPath);
#endif
		}

		void MediaType::on_data_destroy (dinlib::MediaTypes& parData) {
			parData = dinlib::MediaType_Other;
		}

		void MediaType::on_data_create (dinlib::MediaTypes& parData) {
#if defined(WITH_MEDIA_AUTODETECT)
			if (m_force) {
				parData = m_default;
			}
			else {
				const auto guessed_type = mchlib::guess_media_type(std::string(m_search_path));
				parData = guessed_type;
			}
#else
			parData = m_default;
#endif
		}
	} //namespace scantask
} //namespace mchlib
