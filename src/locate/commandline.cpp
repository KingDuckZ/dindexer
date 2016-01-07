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

#include "commandline.hpp"
#include "dindexer-common/commandline.hpp"
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

namespace din {
	bool parse_commandline (int parArgc, char* parArgv[], po::variables_map& parVarMap) {
		po::options_description set_options(ACTION_NAME " options");
		set_options.add_options()
			("case-insensitive,i", "Disable case sensitivity during search")
			("set,s", "Look for matching sets instead of files")
			//("option,o", po::value<std::string>()->default_value("default_value"), "Help message")
			//("option2", po::value<int>(), "Help message")
		;

		po::options_description positional_options("Positional options");
		positional_options.add_options()
			("substring", po::value<std::string>(), "Substring to look for")
		;

		const auto desc = dinlib::get_default_commandline();
		po::options_description all("Available options");
		po::positional_options_description pd;
		all.add(desc).add(positional_options).add(set_options);
		pd.add("substring", 1);//.add("pos_option2", 1);
		try {
			po::store(po::command_line_parser(parArgc, parArgv).options(all).positional(pd).run(), parVarMap);
		}
		catch (const po::validation_error& err) {
			throw dinlib::ValidationError(err);
		}

		po::notify(parVarMap);

		if (dinlib::manage_common_commandline(std::cout, ACTION_NAME, "[options...] <file name, path or substring>", parVarMap, {std::cref(desc), std::cref(set_options)})) {
			return true;
		}

		return false;
	}
} //namespace din
