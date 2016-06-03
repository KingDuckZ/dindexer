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

#include "dindexer-machinery/scantask/setbasic.hpp"
#include <utility>

namespace mchlib {
	namespace scantask {
		SetBasic::SetBasic (std::string&& parName) :
			m_set_name(std::move(parName))
		{
		}

		SetBasic::~SetBasic() noexcept {
		}

		void SetBasic::on_data_destroy (SetRecordDataFull& parData) {
			parData.name.clear();
		}

		void SetBasic::on_data_create (SetRecordDataFull& parData) {
			parData.name = m_set_name;
		}
	} //namespace scantask
} //namespace mchlib
