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

#ifndef id17F1582F16C8478E8D9795BECBF275A3
#define id17F1582F16C8478E8D9795BECBF275A3

#include "dindexer-common/mediatypes.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include <vector>

namespace mchlib {
	enum ContentTypes {
		ContentType_Generic,
		ContentType_Backup,
		ContentType_VideoDVD,
		ContentType_VideoBD,
		ContentType_VideoCD,
		ContentType_Unknown
	};

	template <bool> class SetListingView;

	ContentTypes guess_content_type ( dinlib::MediaTypes parMediaType, const SetListingView<true>& parContent, std::size_t parEntriesCount=0 );
} //namespace mchlib

#endif
