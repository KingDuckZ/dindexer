/* Copyright 2015, Michele Santullo
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

#ifndef id0B4AABA220AE412F81409F63250E4853
#define id0B4AABA220AE412F81409F63250E4853

#include <memory>
#include <string>
#include <utility>
#include <boost/utility/string_ref.hpp>

namespace mchlib {
	using SplitMime = std::pair<boost::string_ref, boost::string_ref>;

	class MimeType {
	public:
		MimeType ( void );
		~MimeType ( void ) noexcept;

		bool initialized ( void ) const;
		const char* analyze ( const std::string& parPath );

	private:
		struct LocalData;

		std::unique_ptr<LocalData> m_local_data;
	};

	SplitMime split_mime ( const std::string& parFull );
} //namespace mchlib

#endif
