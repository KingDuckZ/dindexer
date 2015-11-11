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

#include <boost/lexical_cast.hpp>
#include "pq/databaseexception.hpp"

namespace pq {
	namespace {
		std::string& compose_err_msg ( std::string& parMsg, const std::string& parErr, const std::string& parFile, std::size_t parLine) {
			parMsg += ": ";
			parMsg += parErr;
			parMsg += '\n';
			parMsg += parFile;
			parMsg += ':';
			parMsg += boost::lexical_cast<std::string>(parLine);
			return parMsg;
		}
	} //unnamed namespace

	DatabaseException::DatabaseException (std::string&& parMsg, std::string&& parErr, const std::string& parFile, std::size_t parLine) :
		std::runtime_error(compose_err_msg(parMsg, parErr, parFile, parLine)),
		m_error(std::move(parErr))
	{
	}
} //namespace pq
