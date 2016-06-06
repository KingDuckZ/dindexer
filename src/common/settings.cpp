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

#include "dindexer-common/settings.hpp"
#include "dindexer-common/split_tags.hpp"
#include "dindexerConfig.h"
#include <yaml-cpp/yaml.h>
#include <ciso646>
#include <wordexp.h>
#include <stdexcept>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range_core.hpp>

namespace dinlib {
	namespace {
		std::string expand ( const char* parString );
		std::string find_plugin_by_name ( const std::vector<boost::string_ref>& parSearchPaths, const std::string& parName );
		void throw_if_plugin_failed ( const dindb::BackendPlugin& parPlugin, const std::string& parPluginPath, const std::string& parIntendedName );
	} //unnamed namespace

	void load_settings (const std::string& parPath, dinlib::Settings& parOut, bool parExpand) {
		const std::string path = (parExpand ? expand(parPath.c_str()) : parPath);
		std::string search_paths;

		auto settings = YAML::LoadFile(path);

		if (not settings["backend_name"])
			throw std::runtime_error("No backend_name given in the config file");
		if (settings["backend_paths"])
			search_paths += ":" + settings["backend_paths"].as<std::string>();
		parOut.backend_name = settings["backend_name"].as<std::string>();
		const std::string backend_settings_section = parOut.backend_name + "_settings";
		if (settings[backend_settings_section]) {
			auto settings_node = settings[backend_settings_section];
			const std::string plugin_path = find_plugin_by_name(split_and_trim(search_paths, ':'), parOut.backend_name);
			if (plugin_path.empty())
				throw std::runtime_error(std::string("Unable to find any suitable plugin with the specified name \"") + parOut.backend_name + "\"");
			parOut.backend_plugin = dindb::BackendPlugin(plugin_path, &settings_node);
			throw_if_plugin_failed(parOut.backend_plugin, plugin_path, parOut.backend_name);
		}
	}

	namespace {
		std::string expand (const char* parString) {
			wordexp_t p;
			wordexp(parString, &p, 0);
			char** w = p.we_wordv;
			std::ostringstream oss;
			for (std::size_t z = 0; z < p.we_wordc; ++z) {
				oss << w[z];
			}
			wordfree(&p);
			return oss.str();
		}

		std::string find_plugin_by_name (const std::vector<boost::string_ref>& parSearchPaths, const std::string& parName) {
			using boost::filesystem::path;
			using boost::filesystem::is_directory;
			using boost::filesystem::directory_iterator;
			using boost::filesystem::directory_entry;
			using boost::make_iterator_range;

			for (auto search_path : parSearchPaths) {
				const std::string search_path_cpy(search_path.begin(), search_path.end());
				path curr_path(search_path_cpy);
				auto listing = make_iterator_range(directory_iterator(curr_path), directory_iterator());
				for (const directory_entry& entry : listing) {
					auto current_full_path = entry.path().string();
					if (not is_directory(entry) and dindb::backend_name(current_full_path) == parName)
						return current_full_path;
				}
			}
			return std::string();
		}

		void throw_if_plugin_failed (const dindb::BackendPlugin& parPlugin, const std::string& parPluginPath, const std::string& parIntendedName) {
			if (not parPlugin.is_loaded()) {
				std::ostringstream oss;
				oss << "Unable to load plugin \"" << parIntendedName <<
					"\" found at path \"" << parPluginPath << '"';
				throw std::runtime_error(oss.str());
			}
			if (parPlugin.name() != parIntendedName) {
				std::ostringstream oss;
				oss << "Plugin \"" << parIntendedName << "\" not found." <<
					" Plugin at path \"" << parPluginPath << "\" reports \"" <<
					parPlugin.name() << "\" as its name";
				throw std::runtime_error(oss.str());
			}
			if (parPlugin.max_supported_interface_version() < parPlugin.backend_interface_version()) {
				std::ostringstream oss;
				oss << "Plugin \"" << parPlugin.name() << "\" at path \"" <<
					parPluginPath << "\" uses interface version " << parPlugin.backend_interface_version() <<
					" but the maximum supported interface version is " <<
					parPlugin.max_supported_interface_version();
				throw std::runtime_error(oss.str());
			}
		}
	} //unnamed namespace
} //namespace dinlib
