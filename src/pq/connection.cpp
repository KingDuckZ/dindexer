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

#include "pq/connection.hpp"
#include "pq/databaseexception.hpp"
#include "resultinfo.hpp"
#include <libpq-fe.h>
#include <algorithm>
#include <ciso646>
#include <memory>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <cstring>
#include "libpqtypes.h"
#include <cstdlib>
#include <ctime>
#include <cassert>

namespace pq {
	namespace {
		int call_PQputf (PGparam* parParam, const std::string* parTypes, va_list parArgp) {
			return PQputvf(parParam, nullptr, 0, parTypes->c_str(), parArgp);
		}
	} //unnamed namespace

	struct Connection::LocalData {
		PGconn* connection;
	};

	Connection::Connection (std::string&& parUsername, std::string&& parPasswd, std::string&& parDatabase, std::string&& parAddress, uint16_t parPort) :
		m_localData(new LocalData),
		m_username(std::move(parUsername)),
		m_passwd(std::move(parPasswd)),
		m_database(std::move(parDatabase)),
		m_address(std::move(parAddress)),
		m_port(parPort)
	{
		m_localData->connection = nullptr;
	}

	Connection::Connection (Connection&& parOther) :
		m_localData(std::move(parOther.m_localData)),
		m_username(std::move(parOther.m_username)),
		m_passwd(std::move(parOther.m_passwd)),
		m_database(std::move(parOther.m_database)),
		m_address(std::move(parOther.m_address)),
		m_port(parOther.m_port)
	{
	}

	Connection::~Connection() noexcept {
		if (m_localData) {
			disconnect();
		}
	}

	Connection& Connection::operator= (Connection&& parOther) {
		m_localData = std::move(parOther.m_localData);
		m_username = std::move(parOther.m_username);
		m_passwd = std::move(parOther.m_passwd);
		m_database = std::move(parOther.m_database);
		m_address = std::move(parOther.m_address);
		m_port = parOther.m_port;
		return *this;
	}

	bool Connection::is_connected() const noexcept {
		return m_localData->connection != nullptr;
	}

	void Connection::connect() {
		assert(not is_connected());

		std::unique_ptr<const char*[]> names(new const char*[6]);
		names[0] = "host";
		names[1] = "port";
		names[2] = "dbname";
		names[3] = "user";
		names[4] = "password";
		names[5] = nullptr;

		std::unique_ptr<const char*[]> keywords(new const char*[6]);
		const std::string port(boost::lexical_cast<std::string>(m_port));
		keywords[0] = m_address.c_str();
		keywords[1] = port.c_str();
		keywords[2] = m_database.c_str();
		keywords[3] = m_username.c_str();
		keywords[4] = m_passwd.c_str();
		keywords[5] = nullptr;

		m_localData->connection = PQconnectdbParams(names.get(), keywords.get(), 0);
		if (not m_localData->connection)
			throw DatabaseException("", "Error allocatinng connection object", __FILE__, __LINE__);
		if (PQstatus(m_localData->connection) != CONNECTION_OK) {
			std::string err = error_message();
			disconnect();

			std::ostringstream oss;
			oss << "Unable to connect to database " << m_address << ':' << m_port << " as user \"" << m_username << '"';
			throw DatabaseException(oss.str(), std::move(err), __FILE__, __LINE__);
		}
		this->query("SET NAMES 'utf8'");

		PQinitTypes(m_localData->connection); //Init libpqtypes
	}

	void Connection::disconnect() {
		if (is_connected()) {
			PQclearTypes(m_localData->connection); //clear libpqtypes
			PQfinish(m_localData->connection);
			m_localData->connection = nullptr;
		}
	}

	std::string Connection::error_message() const {
		assert(is_connected());
		return PQerrorMessage(m_localData->connection);
	}

	ResultSet Connection::query (const std::string& parQuery) {
		ResultInfo info(PQexec(m_localData->connection, parQuery.c_str()));
		if (not info.result)
			throw DatabaseException("Error running query", "Error allocating result object", __FILE__, __LINE__);
		const int ress = PQresultStatus(info.result.get());
		if (ress != PGRES_TUPLES_OK && ress != PGRES_COMMAND_OK) {
			throw DatabaseException("Error running query", error_message(), __FILE__, __LINE__);
		}

		return ResultSet(std::move(info));
	}

	std::string Connection::escaped_literal (const std::string& parString) {
		return this->escaped_literal(boost::string_ref(parString));
	}

	std::string Connection::escaped_literal (boost::string_ref parString) {
		typedef std::unique_ptr<char[], void(*)(void*)> PQArrayType;

		PQArrayType clean_str(PQescapeLiteral(m_localData->connection, parString.data(), parString.size()), &PQfreemem);
		return std::string(clean_str.get());
	}

	ResultSet Connection::query_params (const std::string& parQuery, PGParams& parParams) {
		//TODO: make result_format parametric
		int result_format = 0;
		assert(parParams.get());
		ResultInfo info(
			PQparamExec(
				m_localData->connection,
				parParams.get(),
				parQuery.c_str(),
				result_format
			)
		);
		if (not info.result) {
			std::ostringstream oss;
			oss << "Error allocating result object while running \"" << parQuery << "\": " << PQgeterror();
			throw DatabaseException("Error running query", oss.str(), __FILE__, __LINE__);
		}
		const int ress = PQresultStatus(info.result.get());
		if (ress != PGRES_TUPLES_OK && ress != PGRES_COMMAND_OK) {
			throw DatabaseException("Error running query", error_message(), __FILE__, __LINE__);
		}

		return ResultSet(std::move(info));
	}

	auto Connection::make_params (const std::string* parTypes, ...) -> PGParams {
		PGParams retval = make_empty_params();
		va_list argp;

		va_start(argp, parTypes);
		call_PQputf(retval.get(), parTypes, argp);
		va_end(argp);

		return std::move(retval);
	}

	auto Connection::make_empty_params() const -> PGParams {
		assert(is_connected());
		auto ret = PGParams(PQparamCreate(m_localData->connection), &PQparamClear);
		assert(ret.get());
		return std::move(ret);
	}
} //namespace pq
