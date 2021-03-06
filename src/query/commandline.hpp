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

#ifndef id259FD7C96B5049ECB50386F25455FBB2
#define id259FD7C96B5049ECB50386F25455FBB2

#include "dindexer-common/validationerror.hpp"
#include <boost/program_options/variables_map.hpp>

namespace din {
	bool parse_commandline ( int parArgc, char* parArgv[], boost::program_options::variables_map& parVarMap );
} //namespace din

#endif
