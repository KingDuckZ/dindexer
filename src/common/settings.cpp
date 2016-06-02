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

namespace dinlib {
	namespace {
		std::string expand ( const char* parString );
		std::string find_plugin_by_name ( const std::string& parName );
	} //unnamed namespace

	bool load_settings (const std::string& parPath, dinlib::Settings& parOut, bool parExpand) {
		const std::string path = (parExpand ? expand(parPath.c_str()) : parPath);

		try {
			auto settings = YAML::LoadFile(path);

			if (not settings["backend_name"]) {
				return false;
			}
			parOut.backend_name = settings["backend_name"].as<std::string>();
			const std::string backend_settings_section = parOut.backend_name + "_settings";
			if (settings[backend_settings_section]) {
				auto settings_node = settings[backend_settings_section];
				parOut.backend_plugin = dindb::BackendPlugin(find_plugin_by_name(parOut.backend_name), &settings_node);
				return true;
			}
		}
		catch (const std::exception&) {
			return false;
		}

		return false;
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
	} //unnamed namespace
} //namespace dinlib
