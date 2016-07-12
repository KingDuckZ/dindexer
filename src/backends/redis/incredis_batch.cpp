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

#include "incredis_batch.hpp"
#include "helpers/lexical_cast.hpp"
#include <utility>

namespace redis {
	IncRedisBatch::IncRedisBatch (Batch&& parBatch) :
		m_batch(std::move(parBatch))
	{
	}

	void IncRedisBatch::reset() {
		m_batch.reset();
	}
	void IncRedisBatch::throw_if_failed() {
		m_batch.throw_if_failed();
	}

	IncRedisBatch& IncRedisBatch::select (int parIndex) {
		m_batch.run("SELECT", dinhelp::lexical_cast<std::string>(parIndex));
		return *this;
	}

	IncRedisBatch& IncRedisBatch::client_setname (boost::string_ref parName) {
		m_batch.run("CLIENT", "SETNAME", parName);
		return *this;
	}

	IncRedisBatch& IncRedisBatch::hget (boost::string_ref parKey, boost::string_ref parField) {
		m_batch.run("HGET", parKey, parField);
		return *this;
	}

	IncRedisBatch& IncRedisBatch::hincrby (boost::string_ref parKey, boost::string_ref parField, int parInc) {
		m_batch.run("HINCRBY", parKey, parField, dinhelp::lexical_cast<std::string>(parInc));
		return *this;
	}

	IncRedisBatch& IncRedisBatch::srandmember (boost::string_ref parKey, int parCount) {
		m_batch.run("SRANDMEMBER", parKey, dinhelp::lexical_cast<std::string>(parCount));
		return *this;
	}

	IncRedisBatch& IncRedisBatch::srandmember (boost::string_ref parKey) {
		m_batch.run("SRANDMEMBER", parKey);
		return *this;
	}

	IncRedisBatch& IncRedisBatch::script_flush() {
		m_batch.run("SCRIPT", "FLUSH");
		return *this;
	}
} //namespace redis
