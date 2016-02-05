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

#ifndef idC4CC0CB0AA0445999E75D41C853A1D33
#define idC4CC0CB0AA0445999E75D41C853A1D33

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <cstdint>

namespace din {
	class CommandProcessor {
	public:
		using CmdCallback = std::function<bool(const std::vector<std::string>&)>;

		CommandProcessor ( void );
		CommandProcessor ( CommandProcessor&& ) = default;
		~CommandProcessor ( void ) noexcept;

		void add_command ( std::string&& parName, CmdCallback parCallback, uint32_t parExpParams );
		void add_command ( std::string&& parName, std::function<void()> parCallback, uint32_t parExpParams );
		void add_command ( std::string&& parName, std::function<void(const std::string&)> parCallback, uint32_t parExpParams );
		void add_command ( std::string&& parName, std::function<bool()> parCallback, uint32_t parExpParams );
		void add_command ( std::string&& parName, void(*parCallback)(), uint32_t parExpParams );
		void add_command ( std::string&& parName, bool(*parCallback)(), uint32_t parExpParams );
		bool exec_command ( const std::string& parCommand );

	private:
		struct LocalData;

		std::unique_ptr<LocalData> m_local_data;
	};
} //namespace din

#endif
