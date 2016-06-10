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

#ifndef id774125B851514A26BD7C2AD1D804D732
#define id774125B851514A26BD7C2AD1D804D732

#include "reply.hpp"
#include "helpers/has_method.hpp"
#include <boost/iterator/iterator_facade.hpp>
#include <type_traits>
#include <vector>
#include <cstddef>

namespace redis {
	template <typename V, typename ValueFetch>
	class ScanIterator;

	class Command;

	namespace implem {
		template <typename V, typename ValueFetch>
		using ScanIteratorBaseIterator = boost::iterator_facade<ScanIterator<V, ValueFetch>, const V, boost::forward_traversal_tag>;

		class ScanIteratorBaseClass {
		protected:
			explicit ScanIteratorBaseClass ( Command* parCommand );
			~ScanIteratorBaseClass ( void ) noexcept = default;

			bool is_connected ( void ) const;
			RedisReplyType run ( const char* parCommand, long long parScanContext );
			RedisReplyType run ( const char* parCommand, const std::string& parParameter, long long parScanContext );

			bool is_equal ( const ScanIteratorBaseClass& parOther ) const { return m_command == parOther.m_command; }

		private:
			Command* m_command;
		};
	} //namespace implem

	template <typename V, typename ValueFetch>
	class ScanIterator : private implem::ScanIteratorBaseClass, public implem::ScanIteratorBaseIterator<V, ValueFetch> {
		friend class boost::iterator_core_access;
		typedef implem::ScanIteratorBaseIterator<V, ValueFetch> base_iterator;
	public:
		typedef typename base_iterator::difference_type difference_type;
		typedef typename base_iterator::value_type value_type;
		typedef typename base_iterator::pointer pointer;
		typedef typename base_iterator::reference reference;
		typedef typename base_iterator::iterator_category iterator_category;

		ScanIterator ( Command* parCommand, bool parEnd );

	private:
		define_has_method(scan_target, ScanTarget);

		template <typename T>
		RedisReplyType forward_scan_command ( typename std::enable_if<HasScanTargetMethod<T>::value, int>::type parDummy );
		template <typename T>
		RedisReplyType forward_scan_command ( typename std::enable_if<not HasScanTargetMethod<T>::value, int>::type parDummy );

		void increment ( void );
		bool equal ( const ScanIterator& parOther ) const;
		const V& dereference ( void ) const;

		std::vector<RedisReplyType> m_reply;
		long long m_scan_context;
		std::size_t m_curr_index;
	};

	template <typename T>
	struct ScanSingleValues {
		static constexpr const char* command ( void ) { return "SCAN"; }
		static constexpr const std::size_t step = 1;

		static const T& make_value ( const RedisReplyType* parItem );
	};
} //namespace redis

#include "scan_iterator.inl"

#endif
