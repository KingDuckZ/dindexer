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

#include "command.hpp"
#include <cassert>
#include <ciso646>

namespace redis {
	namespace implem {
	} //namespace implem

	template <typename V, typename ValueFetch>
	template <typename Dummy, typename>
	ScanIterator<V, ValueFetch>::ScanIterator (Command* parCommand, bool parEnd) :
		implem::ScanIteratorBaseClass(parCommand),
		implem::ScanIteratorBaseIterator<V, ValueFetch>(),
		ValueFetch(),
		m_reply(),
		m_scan_context(0),
		m_curr_index(0)
	{
		if (not parEnd) {
			m_curr_index = 1; //Some arbitrary value so is_end()==false
			assert(not is_end());
			this->increment();
		}
		else {
			assert(is_end());
		}
	}

	template <typename V, typename ValueFetch>
	template <typename Dummy, typename>
	ScanIterator<V, ValueFetch>::ScanIterator (Command* parCommand, boost::string_ref parKey, bool parEnd) :
		implem::ScanIteratorBaseClass(parCommand),
		implem::ScanIteratorBaseIterator<V, ValueFetch>(),
		ValueFetch(parKey),
		m_reply(),
		m_scan_context(0),
		m_curr_index(0)
	{
		if (not parEnd) {
			m_curr_index = 1; //Some arbitrary value so is_end()==false
			assert(not is_end());
			this->increment();
		}
		else {
			assert(is_end());
		}
	}

	template <typename V, typename ValueFetch>
	bool ScanIterator<V, ValueFetch>::is_end() const {
		return not m_curr_index and m_reply.empty() and not m_scan_context;
	}

	template <typename V, typename ValueFetch>
	void ScanIterator<V, ValueFetch>::increment() {
		assert(not is_end());
		static_assert(ValueFetch::step > 0, "Can't have an increase step of 0");

		if (m_curr_index + 1 < m_reply.size()) {
			++m_curr_index;
		}
		else if (m_curr_index + 1 == m_reply.size() and not m_scan_context)	{
			m_reply.clear();
			m_curr_index = 0;
		}
		else {
			std::vector<RedisReplyType> array_reply;
			long long new_context;

			do {
				auto whole_reply = this->forward_scan_command<ValueFetch>(0);

				array_reply = get_array(whole_reply);
				assert(2 == array_reply.size());
				assert(array_reply.size() % ValueFetch::step == 0);
				new_context = get_integer_autoconv_if_str(array_reply[0]);
			} while (new_context and array_reply.empty());

			const auto variant_array = get_array(array_reply[1]);
			assert(variant_array.size() % ValueFetch::step == 0);
			const std::size_t expected_reply_count = variant_array.size() / ValueFetch::step;
			m_reply.clear();
			m_reply.reserve(expected_reply_count);
			for (std::size_t z = 0; z < variant_array.size(); z += ValueFetch::step) {
				m_reply.push_back(ValueFetch::make_value(variant_array.data() + z));
			}
			assert(expected_reply_count == m_reply.size());
			m_scan_context = new_context;
			m_curr_index = 0;
		}
	}

	template <typename V, typename ValueFetch>
	bool ScanIterator<V, ValueFetch>::equal (const ScanIterator& parOther) const {
		return
			(&parOther == this) or
			(is_end() and parOther.is_end()) or
			(
				not (is_end() or parOther.is_end()) and
				implem::ScanIteratorBaseClass::is_equal(parOther) and
				(m_scan_context == parOther.m_scan_context) and
				(m_curr_index == parOther.m_curr_index) and
				(m_reply.size() == parOther.m_reply.size())
			);
	}

	template <typename V, typename ValueFetch>
	const V& ScanIterator<V, ValueFetch>::dereference() const {
		assert(not m_reply.empty());
		assert(m_curr_index < m_reply.size());

		return m_reply[m_curr_index];
	}

	template <typename V, typename ValueFetch>
	template <typename T>
	RedisReplyType ScanIterator<V, ValueFetch>::forward_scan_command (typename std::enable_if<HasScanTargetMethod<T>::value, int>::type) {
		return implem::ScanIteratorBaseClass::run(T::command(), T::scan_target(), m_scan_context);
	}

	template <typename V, typename ValueFetch>
	template <typename T>
	RedisReplyType ScanIterator<V, ValueFetch>::forward_scan_command (typename std::enable_if<not HasScanTargetMethod<T>::value, int>::type) {
		return implem::ScanIteratorBaseClass::run(T::command(), m_scan_context);
	}

	template <typename T>
	const T& ScanSingleValues<T>::make_value (const RedisReplyType* parItem) {
		assert(parItem);
		return get<T>(*parItem);
	}

	template <typename P, typename A, typename B>
	P ScanPairs<P, A, B>::make_value (const RedisReplyType* parItem) {
		assert(parItem);
		return PairType(get<A>(parItem[0]), get<B>(parItem[1]));
	}

	template <typename P, typename A, typename B>
	boost::string_ref ScanPairs<P, A, B>::scan_target() {
		return m_scan_target;
	}
} //namespace redis
