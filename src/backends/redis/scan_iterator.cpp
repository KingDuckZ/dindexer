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

#include "scan_iterator.hpp"
#include "command.hpp"
#include <cassert>
#include <ciso646>
#include <boost/lexical_cast.hpp>
#include <string>

namespace redis {
	namespace implem {
		ScanIteratorBaseClass::ScanIteratorBaseClass (Command* parCommand) :
			m_command(parCommand)
		{
			assert(m_command);
			assert(m_command->is_connected());
		}

		bool ScanIteratorBaseClass::is_connected() const {
			return m_command and m_command->is_connected();
		}

		RedisReplyType ScanIteratorBaseClass::run (const char* parCommand, long long parScanContext) {
			return m_command->run(parCommand, boost::lexical_cast<std::string>(parScanContext));
		}

		RedisReplyType ScanIteratorBaseClass::run (const char* parCommand, const boost::string_ref& parParameter, long long parScanContext) {
			return m_command->run(parCommand, parParameter, boost::lexical_cast<std::string>(parScanContext));
		}
	} //namespace implem
} //namespace redis
