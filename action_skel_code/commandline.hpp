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

#ifndef id1B7A42F6E46547A6AB0F914E2A91399F
#define id1B7A42F6E46547A6AB0F914E2A91399F

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/errors.hpp>
#include "dindexer-common/mediatypes.hpp"

namespace din {
	class ValidationError : boost::program_options::validation_error {
	public:
		explicit ValidationError ( const boost::program_options::validation_error& parOther );
		~ValidationError ( void ) noexcept = default;

		const std::string& raw_value ( void ) const;
	};

	bool parse_commandline ( int parArgc, char* parArgv[], boost::program_options::variables_map& parVarMap );
} //namespace din

#endif
