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

#include "glob2regex/glob2regex.hpp"
#include "glob_ast.hpp"
#include "render_ast.hpp"
#include <vector>

namespace g2r {
	namespace {
	} //unnamed namespace

	std::string convert (const std::string& parGlob) {
		const auto glob_ast = make_ast(parGlob);
		return render_ast(glob_ast);
	}
} //namespace g2r
