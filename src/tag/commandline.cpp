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

#include "commandline.hpp"
#include "dindexer-common/commandline.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include <vector>

namespace po = boost::program_options;

namespace din {
	bool parse_commandline (int parArgc, char* parArgv[], po::variables_map& parVarMap) {
		po::options_description set_options(ACTION_NAME " options");
		set_options.add_options()
			("delete,d", "Delete tags instead of adding")
			("alltags,a", "Only allowed when --delete is also specified - ignores any given tag list and delete all tags associated to the matching elements instead")
			//("option,o", po::value<std::string>()->default_value("default_value"), "Help message")
			("ids", po::value<std::string>(), "Comma-separated list of IDs of files to be tagged")
			("set,s", po::value<uint32_t>(), "Limit matching to files belonging to the specified set ID")
		;

		po::options_description positional_options("Positional options");
		positional_options.add_options()
			("tags", po::value<std::string>(), "comma-separated tag list")
			("globs", po::value<std::vector<std::string>>(), "List of globs to match against")
		;

		const auto desc = dinlib::get_default_commandline();
		po::options_description all("Available options");
		po::positional_options_description pd;
		all.add(desc).add(positional_options).add(set_options);
		pd.add("tags", 1).add("globs", -1);
		try {
			po::store(po::command_line_parser(parArgc, parArgv).options(all).positional(pd).run(), parVarMap);
		}
		catch (const po::validation_error& err) {
			throw dinlib::ValidationError(err);
		}

		po::notify(parVarMap);

		const char* const help_text = "[options...] tag[,tag2...] (--ids id1[,id2...] | <glob> [glob...])";
		if (dinlib::manage_common_commandline(std::cout, ACTION_NAME, help_text, parVarMap, {std::cref(desc), std::cref(set_options)})) {
			return true;
		}

		return false;
	}
} //namespace din
