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

#ifndef idE1E1650A8CAA4949BD6D4D58BF2599F5
#define idE1E1650A8CAA4949BD6D4D58BF2599F5

#include <vector>
#include <boost/utility/string_ref.hpp>
#include <cstdint>

namespace dinlib {
	struct SettingsDB;
} //namespace dinlib

namespace din {
	void tag_files ( const dinlib::SettingsDB& parDB, const std::vector<uint64_t>& parFiles, const std::vector<boost::string_ref>& parTags );
} //namespace din

#endif