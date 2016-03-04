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
#include "helpers/lengthof.h"
#include "dindexer-common/commandline.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include <algorithm>

namespace po = boost::program_options;

namespace din {
	namespace {
		const char g_allowed_types[] = {
			static_cast<char>(mchlib::MediaType_CDRom),
			static_cast<char>(mchlib::MediaType_Directory),
			static_cast<char>(mchlib::MediaType_DVD),
			static_cast<char>(mchlib::MediaType_BluRay),
			static_cast<char>(mchlib::MediaType_FloppyDisk),
			static_cast<char>(mchlib::MediaType_HardDisk),
			static_cast<char>(mchlib::MediaType_IomegaZip),
			static_cast<char>(mchlib::MediaType_Other)
		};
	} //unnamed namespace

	bool parse_commandline (int parArgc, char* parArgv[], po::variables_map& parVarMap) {
		std::string type_param_help;
		{
			std::ostringstream oss;
			oss << "Default set type. Valid values are ";
			oss << g_allowed_types[0];
			for (std::size_t z = 1; z < lengthof(g_allowed_types); ++z) {
				oss << ", " << g_allowed_types[z];
			}
			oss << '.';
#if defined(WITH_MEDIA_AUTODETECT)
			oss << " Default is 'autodetect'.";
#endif
			type_param_help = oss.str();
		}

		po::options_description set_options(ACTION_NAME " options");
		set_options.add_options()
			("ignore-errors", "Move on even if reading a file fails. Unreadable files are marked as such in the db.")
#if defined(WITH_PROGRESS_FEEDBACK)
			("quiet,q", "Hide progress messages and print nothing at all")
#endif
			("setname,n", po::value<std::string>()->default_value("New set"), "Name to be given to the new set being scanned.")
#if defined(WITH_MEDIA_AUTODETECT)
			("type,t", po::value<char>(), type_param_help.c_str())
#else
			("type,t", po::value<char>()->default_value('V'), type_param_help.c_str())
#endif
		;
		po::options_description positional_options("Positional options");
		positional_options.add_options()
			("search-path", po::value<std::string>(), "Search path")
		;
		po::options_description all("Available options");
		const auto desc = dinlib::get_default_commandline();
		all.add(desc).add(positional_options).add(set_options);
		po::positional_options_description pd;
		pd.add("search-path", 1);//.add("xpath", 1);
		try {
			po::store(po::command_line_parser(parArgc, parArgv).options(all).positional(pd).run(), parVarMap);
		}
		catch (const po::unknown_option& err) {
			throw std::invalid_argument(err.what());
		}

		po::notify(parVarMap);

		if (dinlib::manage_common_commandline(std::cout, ACTION_NAME, "[options...] <search-path>", parVarMap, {std::cref(desc), std::cref(set_options)})) {
			return true;
		}

		if (parVarMap.count("search-path") == 0) {
			throw std::invalid_argument("No search path specified");
		}
#if defined(WITH_MEDIA_AUTODETECT)
		if (parVarMap.count("type")) {
#endif
		if (g_allowed_types + lengthof(g_allowed_types) == std::find(g_allowed_types, g_allowed_types + lengthof(g_allowed_types), parVarMap["type"].as<char>())) {
			throw std::invalid_argument("Invalid value for parameter \"type\"");
		}
#if defined(WITH_MEDIA_AUTODETECT)
		}
#endif
		return false;
	}
} //namespace din
