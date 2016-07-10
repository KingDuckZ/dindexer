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

#ifndef idD83EEBFC927840C6B9F32D61A1D1E582
#define idD83EEBFC927840C6B9F32D61A1D1E582

#include "scan_iterator.hpp"
#include "reply.hpp"
#include "batch.hpp"
#include "redisConfig.h"
#include "script.hpp"
#include <array>
#include <string>
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <vector>
#include <utility>
#include <boost/range/iterator_range_core.hpp>
#include <boost/range/empty.hpp>
#include <boost/utility/string_ref.hpp>
#include <ciso646>
#include <stdexcept>

namespace redis {
	class Command {
	public:
		typedef ScanIterator<ScanSingleValues<std::string>> scan_iterator;
		typedef boost::iterator_range<scan_iterator> scan_range;
		typedef ScanIterator<ScanPairs<std::pair<std::string, std::string>, ScanCommands::HSCAN>> hscan_iterator;
		typedef boost::iterator_range<hscan_iterator> hscan_range;
		typedef ScanIterator<ScanSingleValuesInKey<std::string>> sscan_iterator;
		typedef boost::iterator_range<sscan_iterator> sscan_range;
		typedef ScanIterator<ScanPairs<std::pair<std::string, std::string>, ScanCommands::ZSCAN>> zscan_iterator;
		typedef boost::iterator_range<zscan_iterator> zscan_range;

		Command ( std::string&& parAddress, uint16_t parPort );
		explicit Command ( std::string&& parSocket );
		~Command ( void ) noexcept;

		void connect ( void );
		void wait_for_connect ( void );
		void disconnect ( void );
		void wait_for_disconnect ( void );

		bool is_connected ( void ) const;
		boost::string_ref connection_error ( void ) const;

		Batch make_batch ( void );
		Script make_script ( const std::string& parScript );

		template <typename... Args>
		Reply run ( const char* parCommand, Args&&... parArgs );

		//Single Redis command wrappers
		scan_range scan ( boost::string_ref parPattern=boost::string_ref() );
		hscan_range hscan ( boost::string_ref parKey, boost::string_ref parPattern=boost::string_ref() );
		sscan_range sscan ( boost::string_ref parKey, boost::string_ref parPattern=boost::string_ref() );
		zscan_range zscan ( boost::string_ref parKey, boost::string_ref parPattern=boost::string_ref() );

	private:
		struct LocalData;

		std::unique_ptr<LocalData> m_local_data;
	};

	template <typename... Args>
	Reply Command::run (const char* parCommand, Args&&... parArgs) {
		auto batch = make_batch();
		batch.run(parCommand, std::forward<Args>(parArgs)...);
		batch.throw_if_failed();
		return batch.replies().front();
	}

	template <typename T>
	struct StructAdapt;

	template <typename AS, typename I>
	AS range_conv ( const boost::iterator_range<I>& parRange );

	template <typename AS, typename I>
	inline AS range_as (const boost::iterator_range<I>& parRange) {
		assert(not boost::empty(parRange));
		AS retval;
		const auto success = StructAdapt<AS>::decode(parRange, retval);
		if (not success)
			throw std::runtime_error("Error decoding range");
		return retval;
	};
} //namespace redis

#endif
