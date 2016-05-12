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

#include "glob2regex/exceptions.hpp"
#include <sstream>
#include <cassert>

namespace g2r {
	namespace {
		std::string compose_err_msg (const std::string& parGlob, unsigned int parPosition) {
			assert(parPosition <= parGlob.size());

			std::ostringstream oss;
			oss << "Error parsing glob \"" << parGlob << "\" at character " << (parPosition + 1);

			return oss.str();
		}
	} //unnamed namespace

	ParsingError::ParsingError (const std::string& parGlob, unsigned int parPosition) :
		std::runtime_error(compose_err_msg(parGlob, parPosition)),
		m_glob(parGlob),
		m_position(parPosition)
	{
	}

	const std::string& ParsingError::glob() const noexcept {
		return m_glob;
	}

	unsigned int ParsingError::position() const noexcept {
		return m_position;
	}
} //namespace g2r

