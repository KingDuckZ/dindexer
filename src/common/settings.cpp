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
#include "dindexerConfig.h"
#include <yaml-cpp/yaml.h>
#include <ciso646>
#include <wordexp.h>
#include <stdexcept>
#include <sstream>

namespace dinlib {
	namespace {
		std::string expand ( const char* parString );
		std::string find_plugin_by_name ( const std::string& parName );
		void throw_if_plugin_failed ( const dindb::BackendPlugin& parPlugin, const std::string& parPluginPath, const std::string& parIntendedName );
	} //unnamed namespace

	void load_settings (const std::string& parPath, dinlib::Settings& parOut, bool parExpand) {
		const std::string path = (parExpand ? expand(parPath.c_str()) : parPath);

		auto settings = YAML::LoadFile(path);

		if (not settings["backend_name"]) {
			throw std::runtime_error("No backend_name given in the config file");
		}
		parOut.backend_name = settings["backend_name"].as<std::string>();
		const std::string backend_settings_section = parOut.backend_name + "_settings";
		if (settings[backend_settings_section]) {
			auto settings_node = settings[backend_settings_section];
			const std::string plugin_path = find_plugin_by_name(parOut.backend_name);
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

		std::string find_plugin_by_name (const std::string& parName) {
			//assert(false); //not implemented
			//TODO: write a proper implementation
			std::string path = ACTIONS_SEARCH_PATH;
			path += "/backends/postgresql/libdindexer-backend-postgresql.so";

			assert(dindb::backend_name(path) == parName);
			return path;
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
