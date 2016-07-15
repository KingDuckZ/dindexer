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

#ifndef id25B0BCA6D9464754920E1BC7C5D9DB57
#define id25B0BCA6D9464754920E1BC7C5D9DB57

#include "dindexer-machinery/scantask/leanbase.hpp"
#include "dindexer-machinery/scantask/base.hpp"
#include "dindexer-machinery/mediatypes.hpp"
#include <string>
#include <memory>

namespace mchlib {
	struct SetRecordDataFull;

	namespace scantask {
		class MediaType : public LeanBase<SetRecordDataFull> {
		public:
			using SetTaskType = std::shared_ptr<LeanBase<SetRecordDataFull>>;

			MediaType ( SetTaskType parSet, char parDefault, bool parForce, std::string parSearchPath );
			virtual ~MediaType ( void ) noexcept = default;

		private:
			virtual void on_data_fill ( void ) override;
			virtual SetRecordDataFull& on_data_get ( void ) override;

			SetTaskType m_set_task;
			MediaTypes m_default;
			std::string m_search_path;
#if defined(WITH_MEDIA_AUTODETECT)
			bool m_force;
#endif
		};
	} //namespace scantask
} //namespace mchlib

#endif
