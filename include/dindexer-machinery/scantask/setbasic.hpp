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

#ifndef idB323CEFC89C2468CA0A341C312C6C2C0
#define idB323CEFC89C2468CA0A341C312C6C2C0

#include "dindexer-machinery/scantask/base.hpp"
#include "dindexer-machinery/recorddata.hpp"
#include <string>

namespace mchlib {
	namespace scantask {
		class SetBasic : public Base<SetRecordDataFull> {
			typedef Base<SetRecordDataFull> ParentType;
		public:
			explicit SetBasic ( std::string&& parName );
			virtual ~SetBasic ( void ) noexcept;

		private:
			virtual void on_data_destroy ( SetRecordDataFull& parData ) override;
			virtual void on_data_create ( SetRecordDataFull& parData ) override;

			std::string m_set_name;
		};
	} //namespace scantask
} //namespace mchlib

#endif
