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

#ifndef id982AF1D5C59C415584F56C1E6DDFE55E
#define id982AF1D5C59C415584F56C1E6DDFE55E

#include <ciso646>
#include <cassert>

#if !defined(NDEBUG)
#	 define LEANBASE_ASSERT_REENTRANCY
#endif

namespace mchlib {
	namespace scantask {
#if defined(LEANBASE_ASSERT_REENTRANCY)
		struct AutoSetBool {
			explicit AutoSetBool ( bool* parBool ) :
				m_bool(parBool)
			{
				assert(m_bool);
				assert(not *m_bool);
				*m_bool = true;
			}
			~AutoSetBool ( void ) noexcept {
				*m_bool = false;
			}

			bool* m_bool;
		};
#endif

		template <typename T>
		class LeanBase {
		protected:
			LeanBase ( void );
			virtual ~LeanBase ( void ) noexcept = default;

			void unset_data_created ( void );

		public:
			T& get_or_create ( void );

		private:
			virtual void on_data_fill ( void ) = 0;
			virtual T& on_data_get ( void ) = 0;

			bool m_data_created;
#if defined(LEANBASE_ASSERT_REENTRANCY)
			bool m_inside_call;
#endif
		};

		template <typename T>
		LeanBase<T>::LeanBase() :
			m_data_created(false)
#if defined(LEANBASE_ASSERT_REENTRANCY)
			, m_inside_call(false)
#endif
		{
		}

		template <typename T>
		T& LeanBase<T>::get_or_create() {
#if defined(LEANBASE_ASSERT_REENTRANCY)
			assert(not m_inside_call);
			AutoSetBool auto_bool(&m_inside_call);
#endif

			if (not m_data_created) {
				m_data_created = true;
				this->on_data_fill();
			}
			return this->on_data_get();
		}

		template <typename T>
		void LeanBase<T>::unset_data_created() {
			assert(m_data_created);
			m_data_created = false;
		}
	} //namespace scantask
} //namespace mchlib

#endif
