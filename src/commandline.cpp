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
#include "dindexerConfig.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <algorithm>

#define STRINGIZE_IMPL(s) #s
#define STRINGIZE(s) STRINGIZE_IMPL(s)

#if defined(lengthof)
#	undef lengthof
#endif
//http://stackoverflow.com/questions/4415524/common-array-length-macro-for-c#4415646
#define lengthof(x) ((sizeof(x)/sizeof(0[x])) / ((std::size_t)(!(sizeof(x) % sizeof(0[x])))))

namespace po = boost::program_options;

namespace din {
	namespace {
		const char g_allowed_types[] = {
			'D', //Directory
			'V', //DVD
			'B', //BluRay
			'F', //Floppy Disk
			'H', //Hard Disk
			'Z', //Iomega Zip
			'O'  //Other
		};
		const char* const g_version_string =
			PROGRAM_NAME " v"
			STRINGIZE(VERSION_MAJOR) "."
			STRINGIZE(VERSION_MINOR) "."
			STRINGIZE(VERSION_PATCH)
#if VERSION_BETA
			"b"
#endif
			;
	} //unnamed namespace

	bool parse_commandline (int parArgc, char* parArgv[], po::variables_map& parVarMap) {
		po::options_description desc("General");
		desc.add_options()
			("help,h", "Produces this help message")
			("version", "Prints the program's version and quits")
			//("dump-raw,D", po::value<std::string>(), "Saves the retrieved html to the named file; use - for stdout")
		;
		po::options_description set_options("Set options");
		set_options.add_options()
			("setname,n", po::value<std::string>()->default_value("New set"), "Name to be given to the new set being scanned.")
			("type,t", po::value<char>()->default_value('V'), "Default set type. Valid values are B, D, F, H, O, V, Z.")
		;
		po::options_description positional_options("Positional options");
		positional_options.add_options()
			("search-path", po::value<std::string>(), "Search path")
		;
		po::options_description all("Available options");
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

		if (parVarMap.count("help")) {
#if !defined(NDEBUG)
			std::cout << "*******************\n";
			std::cout << "*** DEBUG BUILD ***\n";
			std::cout << "*******************\n";
			std::cout << '\n';
#endif
			po::options_description visible("Available options");
			visible.add(desc).add(set_options);
			std::cout << PROGRAM_NAME << " Copyright (C) 2015  Michele Santullo\n";
			std::cout << "This program comes with ABSOLUTELY NO WARRANTY.\n"; //for details type `show w'.
			std::cout << "This is free software, and you are welcome to\n";
			std::cout << "redistribute it under certain conditions.\n"; //type `show c' for details.
			std::cout << '\n';
			std::cout << "Usage: " << PROGRAM_NAME << " [options...] <search-path>\n";
			std::cout << visible;
			return true;
		}
		else if (parVarMap.count("version")) {
			std::cout << g_version_string << '\n';
			return true;
		}

		if (parVarMap.count("search-path") == 0) {
			throw std::invalid_argument("No search path specified");
		}
		if (g_allowed_types + lengthof(g_allowed_types) == std::find(g_allowed_types, g_allowed_types + lengthof(g_allowed_types), parVarMap["type"].as<char>())) {
			throw std::invalid_argument("Invalid value for parameter \"type\"");
		}
		return false;
	}
} //namespace din
