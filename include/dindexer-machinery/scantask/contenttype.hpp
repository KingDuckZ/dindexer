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

#ifndef id148DBED10A0B45238E810683656BA7D5
#define id148DBED10A0B45238E810683656BA7D5

#include "dindexer-machinery/scantask/base.hpp"
#include "dindexer-machinery/guess_content_type.hpp"
#include "dindexer-machinery/mediatypes.hpp"
#include <memory>
#include <vector>

namespace mchlib {
	struct FileRecordData;

	namespace scantask {
		class ContentType : public Base<mchlib::ContentTypes> {
		public:
			using DirTreeTaskPtr = std::shared_ptr<Base<std::vector<FileRecordData>>>;
			using MediaTypeTaskPtr = std::shared_ptr<Base<mchlib::MediaTypes>>;

			ContentType ( DirTreeTaskPtr parDirTree, MediaTypeTaskPtr parMediaType );

		private:
			virtual void on_data_destroy ( mchlib::ContentTypes& parData ) override;
			virtual void on_data_create ( mchlib::ContentTypes& parData ) override;

			DirTreeTaskPtr m_dir_tree;
			MediaTypeTaskPtr m_media_type;
		};
	} //namespace scantask
} //namespace mchlib

#endif
