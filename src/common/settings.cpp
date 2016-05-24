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
#include <yaml-cpp/yaml.h>
#include <ciso646>
#include <wordexp.h>

namespace YAML {
	template<>
	struct convert<dindb::Settings> {
		static Node encode (const dindb::Settings& parSettings) {
			Node node;
			node["address"] = parSettings.address;
			node["username"] = parSettings.username;
			node["password"] = parSettings.password;
			node["port"] = parSettings.port;
			node["dbname"] = parSettings.dbname;
			return node;
		}

		static bool decode (const Node& parNode, dindb::Settings& parSettings) {
			if (not parNode.IsMap() or parNode.size() != 5) {
				return false;
			}

			parSettings.address = parNode["address"].as<std::string>();
			parSettings.username = parNode["username"].as<std::string>();
			parSettings.password = parNode["password"].as<std::string>();
			parSettings.dbname = parNode["dbname"].as<std::string>();
			parSettings.port = parNode["port"].as<uint16_t>();
			return true;
		}
	};
} //namespace YAML

namespace dinlib {
	namespace {
		std::string expand ( const char* parString );
	} //unnamed namespace

	bool load_settings (const std::string& parPath, dinlib::Settings& parOut, bool parExpand) {
		const std::string path = (parExpand ? expand(parPath.c_str()) : parPath);

		try {
			auto settings = YAML::LoadFile(path);

			if (settings["db_settings"]) {
				parOut.db = settings["db_settings"].as<dindb::Settings>();
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
	} //unnamed namespace
} //namespace dinlib
