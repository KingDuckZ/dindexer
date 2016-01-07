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

#include "pq/resultset.hpp"
#include "resultinfo.hpp"
#include "pq/databaseexception.hpp"
#include <libpq-fe.h>
#include <map>
#include <cassert>
#include <ciso646>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <boost/bind.hpp>
#include "libpqtypes.h"

namespace pq {
	namespace {
		int cast_col_index_or_throw (std::size_t parIndex, std::size_t parCount, const char* parFile, int parLine) {
			if (parIndex >= parCount) {
				std::ostringstream oss;
				oss << "Requested column at index " << parIndex << " is out of range (column count is " << parCount << ')';
				throw DatabaseException("Error retrieving column", oss.str(), parFile, parLine);
			}
			return static_cast<int>(parIndex);
		}

		int conv_col_index_or_throw (const std::string& parIndex, const std::map<std::string, int>& parCols, const char* parFile, int parLine) {
			const auto num_index_it = parCols.find(parIndex);
			if (parCols.end() == num_index_it) {
				throw DatabaseException("Error retrieving column", "Requested column \"" + parIndex + "\" doesn't exist", parFile, parLine);
			}

			return num_index_it->second;
		}

		std::string binary_to_string (const char* parValue, Oid parType, int parLength) {
			using boost::lexical_cast;

			static_cast<void>(parLength);
			assert(parValue);
			assert(parLength > 0);

			//TODO: use libpqtypes
			switch (parType) {
			case 21:
				return lexical_cast<std::string>(*reinterpret_cast<const uint16_t*>(parValue));
			default:
				//not implemented
				assert(false);
				return std::string();
			}
		}
	} //unnamed namespace

	struct Row::LocalData {
		LocalData ( const PGresult* parResult, std::size_t parRow, const std::map<std::string, int>* parCols, std::size_t parColCount ) :
			result(parResult),
			columns(parCols),
			row_index(parRow),
			col_count(parColCount)
		{
			assert((result and columns) or (not (result or columns)));
		}

		const PGresult* const result;
		const std::map<std::string, int>* const columns;
		const std::size_t row_index;
		const std::size_t col_count;
	};

	struct ResultSet::LocalData {
		typedef ResultInfo::PGResultPtr PGResultPtr;

		explicit LocalData ( PGResultPtr&& parResult ) :
			info(std::move(parResult), &columns)
		{
		}

		ResultInfo info;
		std::map<std::string, int> columns;
	};

	Row::Row (const ResultInfo* parResult, std::size_t parRow) :
		m_localData(new LocalData(parResult->result.get(), parRow, parResult->column_mappings, (parResult->column_mappings ? parResult->column_mappings->size() : 0)))
	{
		assert(not m_localData->result or static_cast<std::size_t>(std::max(0, PQnfields(m_localData->result))) == m_localData->col_count);
	}

	Row::Row (Row&& parOther) :
		m_localData(std::move(parOther.m_localData))
	{
	}

	Row::~Row() noexcept {
	}

	std::string Row::operator[] (const std::string& parIndex) const {
		return get_as_str_assume_valid(conv_col_index_or_throw(parIndex, *m_localData->columns, __FILE__, __LINE__));
	}

	std::size_t Row::size() const {
		return m_localData->col_count;
	}

	bool Row::empty() const {
		return size() == 0;
	}

	std::string Row::operator[] (std::size_t parIndex) const {
		return get_as_str_assume_valid(cast_col_index_or_throw(parIndex, m_localData->col_count, __FILE__, __LINE__));
	}

	std::string Row::get_as_str_assume_valid (int parIndex) const {
		const int num_index = parIndex;
		const auto row_index = static_cast<int>(m_localData->row_index);
		const auto format = PQfformat(m_localData->result, num_index);
		const int data_len = PQgetlength(m_localData->result, row_index, num_index);
		switch (format) {
		case 0:
			//textual data
			return std::string(PQgetvalue(m_localData->result, row_index, num_index), data_len);
		case 1:
			//binary data
			if (is_null(num_index)) {
				return std::string();
			}
			else {
				return binary_to_string(
					PQgetvalue(m_localData->result, row_index, num_index),
					PQftype(m_localData->result, num_index),
					data_len
				);
			}
		default:
			//unknown
			assert(false);
			return std::string();
		}
	}

	bool Row::is_null (std::size_t parIndex) const {
		const auto num_index = cast_col_index_or_throw(parIndex, m_localData->col_count, __FILE__, __LINE__);
		const auto row_index = static_cast<int>(m_localData->row_index);
		return (1 == PQgetisnull(m_localData->result, row_index, num_index) ? true : false);
	}

	bool Row::is_null (const std::string& parIndex) const {
		const auto num_index = conv_col_index_or_throw(parIndex, *m_localData->columns, __FILE__, __LINE__);
		const auto row_index = static_cast<int>(m_localData->row_index);
		return (1 == PQgetisnull(m_localData->result, row_index, num_index) ? true : false);
	}

	Row::const_iterator Row::begin() const {
		return boost::make_transform_iterator(
			boost::counting_iterator<std::size_t>(0),
			boost::bind(static_cast<std::string (Row::*)(std::size_t) const>(&Row::operator[]), this, _1)
		);
	}

	Row::const_iterator Row::end() const {
		return boost::make_transform_iterator(
			boost::counting_iterator<std::size_t>(m_localData->col_count),
			boost::bind(static_cast<std::string (Row::*)(std::size_t) const>(&Row::operator[]), this, _1)
		);
	}


	ResultSet::ResultSet (ResultInfo&& parResult) :
		m_localData(new LocalData(std::move(parResult.result)))
	{
		const auto result = m_localData->info.result.get();
		const int column_count = PQnfields(result);
		std::string col_name;
		for (int z = 0; z < column_count; ++z) {
			m_localData->columns[PQfname(result, z)] = z;
		}
	}

	ResultSet::ResultSet (ResultSet&& parOther) :
		m_localData(std::move(parOther.m_localData))
	{
	}

	ResultSet::~ResultSet() noexcept {
	}

	Row ResultSet::operator[] (std::size_t parIndex) const {
		if (parIndex >= size())
			throw DatabaseException("Out of range", "Column index " + boost::lexical_cast<std::string>(parIndex) + " is out of range (column count is " + boost::lexical_cast<std::string>(size()) + ')', __FILE__, __LINE__);

		return Row(&m_localData->info, parIndex);
	}

	std::size_t ResultSet::size() const {
		assert(m_localData->info.result);
		const auto retval = PQntuples(m_localData->info.result.get());
		assert(retval >= 0);
		return static_cast<std::size_t>(std::max(0, retval));
	}

	bool ResultSet::empty() const {
		return size() == 0;
	}

	ResultSet::const_iterator ResultSet::begin() const {
		return boost::make_transform_iterator(
			boost::counting_iterator<std::size_t>(0),
			boost::bind(static_cast<Row (ResultSet::*)(std::size_t) const>(&ResultSet::operator[]), this, _1)
		);
	}

	ResultSet::const_iterator ResultSet::end() const {
		return boost::make_transform_iterator(
			boost::counting_iterator<std::size_t>(size()),
			boost::bind(static_cast<Row (ResultSet::*)(std::size_t) const>(&ResultSet::operator[]), this, _1)
		);
	}
} //namespace pq
