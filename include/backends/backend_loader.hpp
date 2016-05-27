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

#ifndef id756A258A98B24B0DB2529BCEEC5137E2
#define id756A258A98B24B0DB2529BCEEC5137E2

#include <string>
#include <memory>
#include <boost/utility/string_ref.hpp>

namespace YAML {
	class Node;
} //namespace YAML

namespace dindb {
	class Backend;

	using BackendPtr = std::unique_ptr<dindb::Backend, void(*)(dindb::Backend*)>;

	class BackendPlugin {
	public:
		BackendPlugin ( void );
		BackendPlugin ( BackendPlugin&& ) = default;
		BackendPlugin ( const std::string& parSOPath, const YAML::Node* parConfig );
		~BackendPlugin ( void ) noexcept;

		const boost::string_ref& name ( void ) const;
		Backend& backend ( void );
		const Backend& backend ( void ) const;
		bool is_loaded ( void ) const;

		BackendPlugin& operator= ( BackendPlugin&& ) = default;

	private:
		using SoHandle = std::unique_ptr<void, int(*)(void*)>;

		SoHandle m_lib;
		BackendPtr m_backend;
		boost::string_ref m_name;
	};

	std::string backend_name ( const std::string& parSOPath );
} //namespace dindb

#endif
