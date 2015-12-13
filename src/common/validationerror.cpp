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

#include "dindexer-common/validationerror.hpp"
#include <boost/program_options/errors.hpp>

namespace po = boost::program_options;

namespace dinlib {
	ValidationError::ValidationError (const po::validation_error& parOther) :
		po::validation_error(parOther)
	{
	}

	const std::string& ValidationError::raw_value() const {
		auto it_ret = m_substitutions.find("value");
		return it_ret->second;
	}
} //namespace dinlib
