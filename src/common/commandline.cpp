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

#include "dindexer-common/commandline.hpp"
#include "dindexerConfig.h"
#include "gitinfo.h"
#include "helpers/lengthof.h"
#include "helpers/stringize.h"
#include "helpers/infix_iterator.hpp"
#include <boost/program_options.hpp>
#include <ostream>
#include <utility>
#include <cstdint>
#include <algorithm>

namespace po = boost::program_options;

namespace dinlib {
	namespace {
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

	po::options_description get_default_commandline() {
		po::options_description desc("General");
		desc.add_options()
			("help,h", "Produces this help message")
			("version", "Prints the program's version and quits")
			//("dump-raw,D", po::value<std::string>(), "Saves the retrieved html to the named file; use - for stdout")
		;
		return desc;
	}

	void print_commandline_help (std::ostream& parStream, const std::string& parSummary, const boost::program_options::options_description& parOpts) {
#if !defined(NDEBUG)
			parStream << "*******************\n";
			parStream << "*** DEBUG BUILD ***\n";
			parStream << "*******************\n";
			parStream << '\n';
#endif
			parStream << PROGRAM_NAME << " Copyright (C) ";
			const uint32_t copyright_years[] = { COPYRIGHT_YEARS_LIST };
			std::copy(copyright_years, copyright_years + lengthof(copyright_years), infix_ostream_iterator<uint32_t>(parStream, ", "));
			parStream << "  Michele Santullo\n";
			parStream << "This program comes with ABSOLUTELY NO WARRANTY.\n"; //for details type `show w'.
			parStream << "This is free software, and you are welcome to\n";
			parStream << "redistribute it under certain conditions.\n"; //type `show c' for details.
			parStream << '\n';
			parStream << "Usage: " << PROGRAM_NAME << ' ' << parSummary << '\n';
			parStream << parOpts;
	}

	void print_commandline_version (std::ostream& parStream) {
		parStream << g_version_string << '\n';
		parStream << "Rev " << VERSION_GIT << '\n';
	}

	bool manage_common_commandline (std::ostream& parStream, const char* parAction, const char* parUsage, const boost::program_options::variables_map& parVarMap, const boost::program_options::options_description& parOpts) {
		if (parVarMap.count("help")) {
			std::ostringstream oss_summary;
			oss_summary << parAction << ' ' << parUsage;

			print_commandline_help(parStream, oss_summary.str(), parOpts);
			return true;
		}
		else if (parVarMap.count("version")) {
			print_commandline_version(parStream);
			return true;
		}
		return false;
	}

	bool manage_common_commandline (std::ostream& parStream, const char* parAction, const char* parUsage, const boost::program_options::variables_map& parVarMap, std::initializer_list<std::reference_wrapper<const boost::program_options::options_description>> parOpts) {
		boost::program_options::options_description visible("Available options");
		for (auto opt : parOpts) {
			visible.add(opt);
		}
		return manage_common_commandline(parStream, parAction, parUsage, parVarMap, visible);
	}
} //namespace dinlib

