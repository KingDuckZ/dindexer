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
#include "dindexer-common/settings.hpp"
#include "dindexerConfig.h"
#include "dindexer-core/split_tags.hpp"
#include "glob2regex/glob2regex.hpp"
#include "enum.h"
#include <iostream>
#include <ciso646>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <iterator>

//Leave this out of the unnamed namespace to avoid all the "unused function" warnings
BETTER_ENUM(TaggingMode, char,
	Glob, ID
);

namespace {
	std::vector<std::string> globs_to_regex_list (const std::vector<std::string>& parGlobs) {
		std::vector<std::string> retval;
		retval.reserve(parGlobs.size());
		std::transform(
			parGlobs.begin(),
			parGlobs.end(),
			std::back_inserter(retval),
			[](const std::string& s) { return g2r::convert(s, false); }
		);
		return retval;
	}

	dindb::GroupIDType make_owner_set_info (const boost::program_options::variables_map& parVM) {
		if (parVM.count("set"))
			return parVM["set"].as<dindb::GroupIDType>();
		else
			return dindb::InvalidGroupID;
	}

	int tag_files (dindb::Backend& parDB, TaggingMode parMode, const boost::program_options::variables_map& parVM, const std::vector<boost::string_ref>& parTags) {
		using boost::lexical_cast;
		using boost::string_ref;

		const auto set_info = make_owner_set_info(parVM);

		switch (parMode) {
		case TaggingMode::ID:
		{
			auto ids_string = dincore::split_tags(parVM["ids"].as<std::string>());
			std::vector<uint64_t> ids;
			ids.reserve(ids_string.size());
			std::transform(ids_string.begin(), ids_string.end(), std::back_inserter(ids), &lexical_cast<uint64_t, string_ref>);
			parDB.tag_files(ids, parTags, set_info);
			return 0;
		}

		case TaggingMode::Glob:
		{
			const auto regexes(globs_to_regex_list(parVM["globs"].as<std::vector<std::string>>()));
			parDB.tag_files(regexes, parTags, set_info);
			return 0;
		}

		default:
			assert(false);
			return 1;
		}
	}

	int delete_tags (dindb::Backend& parDB, TaggingMode parMode, const boost::program_options::variables_map& parVM, const std::vector<boost::string_ref>& parTags) {
		using boost::lexical_cast;
		using boost::string_ref;

		assert(parVM.count("delete"));

		switch (parMode) {
		case TaggingMode::ID:
		{
			auto ids_string = dincore::split_tags(parVM["ids"].as<std::string>());
			std::vector<uint64_t> ids;
			ids.reserve(ids_string.size());
			std::transform(ids_string.begin(), ids_string.end(), std::back_inserter(ids), &lexical_cast<uint64_t, string_ref>);
			if (parVM.count("alltags"))
				parDB.delete_all_tags(ids, make_owner_set_info(parVM));
			else
				parDB.delete_tags(ids, parTags, make_owner_set_info(parVM));
			return 0;
		}

		case TaggingMode::Glob:
		{
			const auto regexes(globs_to_regex_list(parVM["globs"].as<std::vector<std::string>>()));
			if (parVM.count("alltags"))
				parDB.delete_all_tags(regexes, make_owner_set_info(parVM));
			else
				parDB.delete_tags(regexes, parTags, make_owner_set_info(parVM));
			return 0;
		}

		default:
			assert(false);
			return 1;
		}
	}
} //unnamed namespace

int main (int parArgc, char* parArgv[]) {
	using boost::program_options::variables_map;

	variables_map vm;
	try {
		if (din::parse_commandline(parArgc, parArgv, vm)) {
			return 0;
		}
	}
	catch (const std::invalid_argument& err) {
		std::cerr << err.what() << "\nUse --help for help" << std::endl;
		return 2;
	}

	const bool id_mode = static_cast<bool>(vm.count("ids"));
	const bool glob_mode = static_cast<bool>(vm.count("globs"));
	if (not id_mode and not glob_mode) {
		std::cerr << "No IDs or glob specified\n";
		return 2;
	}
	else if (id_mode and glob_mode) {
		std::cerr << "Can't specify both a glob and IDs, please only use one of them\n";
		return 2;
	}
	assert(id_mode xor glob_mode);

	dinlib::Settings settings;
	try {
		dinlib::load_settings(CONFIG_FILE_PATH, settings);
	}
	catch (const std::runtime_error& err) {
		std::cerr << "Can't load settings from " << CONFIG_FILE_PATH << ":\n";
		std::cerr << err.what() << '\n';
		return 1;
	}

	const auto master_tags_string = vm["tags"].as<std::string>();
	const std::vector<boost::string_ref> tags = dincore::split_tags(master_tags_string);
	const auto mode = (glob_mode ? TaggingMode::Glob : TaggingMode::ID);

	if (not vm.count("delete"))
		return tag_files(settings.backend_plugin.backend(), mode, vm, tags);
	else
		return delete_tags(settings.backend_plugin.backend(), mode, vm, tags);
}
