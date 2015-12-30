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

#ifndef idA54B379097864A0BA1B7F9C42B74EAAD
#define idA54B379097864A0BA1B7F9C42B74EAAD

#include <cstddef>
#include <string>
#include <memory>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/function.hpp>

namespace pq {
	class Connection;
	struct ResultInfo;

	class Row {
	public:
		typedef boost::transform_iterator<boost::function<std::string(std::size_t)>, boost::counting_iterator<std::size_t>> const_iterator;

		Row ( const ResultInfo* parResult, std::size_t parRow );
		Row ( const Row& ) = delete;
		Row ( Row&& parOther );
		~Row ( void ) noexcept;

		bool empty ( void ) const;
		std::size_t size ( void ) const;
		std::string operator[] ( const std::string& parIndex ) const;
		std::string operator[] ( std::size_t parIndex ) const;
		Row& operator= ( const Row& ) = delete;
		Row& operator= ( Row&& ) = delete;

		const_iterator begin ( void ) const;
		const_iterator end ( void ) const;
		const_iterator cbegin ( void ) const { return begin(); }
		const_iterator cend ( void ) const { return end(); }

	private:
		struct LocalData;

		std::unique_ptr<LocalData> m_localData;
	};

	class ResultSet {
	public:
		typedef boost::transform_iterator<boost::function<Row(std::size_t)>, boost::counting_iterator<std::size_t>> const_iterator;

		explicit ResultSet ( ResultInfo&& parResult );
		ResultSet ( const ResultSet& ) = delete;
		ResultSet ( ResultSet&& parOther );
		~ResultSet ( void ) noexcept;

		bool empty ( void ) const;
		std::size_t size ( void ) const;
		Row operator[] ( const std::size_t parIndex ) const;

		const_iterator begin ( void ) const;
		const_iterator end ( void ) const;
		const_iterator cbegin ( void ) const { return begin(); }
		const_iterator cend ( void ) const { return end(); }

	private:
		struct LocalData;

		std::unique_ptr<LocalData> m_localData;
	};
} //namespace pq

#endif
