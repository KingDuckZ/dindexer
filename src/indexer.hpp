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

#ifndef idE555EF56730442C1ADDC7B2AE7A9340E
#define idE555EF56730442C1ADDC7B2AE7A9340E

#include <memory>
#include <string>

#if !defined(NDEBUG)
#	define INDEXER_VERBOSE
#endif

#if defined(WITH_PROGRESS_FEEDBACK)
namespace std {
	class condition_variable;
} //namespace std
#endif

namespace din {
	struct DinDBSettings;

	class Indexer {
	public:
		explicit Indexer ( const DinDBSettings& parDBSettings );
		Indexer ( Indexer&& ) = default;
		Indexer ( const Indexer& ) = delete;
		~Indexer ( void ) noexcept;

		bool add_path ( const char* parPath, int parLevel, bool parIsDir, bool parIsSymlink );
#if defined(INDEXER_VERBOSE)
		void dump ( void ) const;
#endif

		std::size_t total_items ( void ) const;
		std::string operator[] ( std::size_t parIndex ) const;
#if defined(WITH_PROGRESS_FEEDBACK)
		std::size_t processed_items ( void ) const;
		std::string current_item ( void ) const;
		std::condition_variable& step_notify ( void );
#endif
		void calculate_hash ( void );
		void add_to_db ( const std::string& parSetName, char parType ) const;
		bool empty ( void ) const;

	private:
		struct LocalData;

		std::unique_ptr<LocalData> m_local_data;
	};
} //namespace din

#endif