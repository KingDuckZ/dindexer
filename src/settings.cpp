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

#include "settings.hpp"
#include <yaml-cpp/yaml.h>
#include <ciso646>

namespace YAML {
	template<>
	struct convert<din::DinDBSettings> {
		static Node encode (const din::DinDBSettings& parSettings) {
			Node node;
			node["address"] = parSettings.address;
			node["username"] = parSettings.username;
			node["password"] = parSettings.password;
			node["port"] = parSettings.port;
			node["dbname"] = parSettings.dbname;
			return node;
		}

		static bool decode (const Node& parNode, din::DinDBSettings& parSettings) {
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

namespace din {
	bool load_settings (const std::string& parPath, DinDBSettings& parOut) {
		try {
			auto settings = YAML::LoadFile(parPath);

			if (settings["db_settings"]) {
				parOut = settings["db_settings"].as<DinDBSettings>();
				return true;
			}
		}
		catch (const std::exception&) {
			return false;
		}

		return false;
	}
} //namespace din
