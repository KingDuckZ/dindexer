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

#ifndef idA71E357A577B4A55AB6A713BCC884CEB
#define idA71E357A577B4A55AB6A713BCC884CEB

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <ostream>
#include <string>
#include <initializer_list>
#include <functional>

namespace dinlib {
	boost::program_options::options_description get_default_commandline ( void );
	void print_commandline_help ( std::ostream& parStream, const std::string& parSummary, const boost::program_options::options_description& parOpts  );
	void print_commandline_version ( std::ostream& parStream );

	bool manage_common_commandline ( std::ostream& parStream, const char* parAction, const char* parUsage, const boost::program_options::variables_map& parVarMap, const boost::program_options::options_description& parOpts );

	bool manage_common_commandline ( std::ostream& parStream, const char* parAction, const char* parUsage, const boost::program_options::variables_map& parVarMap, std::initializer_list<std::reference_wrapper<const boost::program_options::options_description>> parOpts );
} //namespace dinlib

#endif
