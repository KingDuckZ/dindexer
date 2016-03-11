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

#ifndef idCB253C1A5AFA46A18B8878ED4072CD96
#define idCB253C1A5AFA46A18B8878ED4072CD96

#include "dindexer-machinery/scantask/leanbase.hpp"
#include <ciso646>
#include <cassert>

namespace mchlib {
	namespace scantask {
		template <typename T>
		class Base : public LeanBase<T> {
		protected:
			Base ( void );
			virtual ~Base ( void ) noexcept = default;

		public:
			void clear_data ( void );

		private:
			virtual void on_data_destroy ( T& parData ) = 0;
			virtual void on_data_create ( T& parData ) = 0;

			virtual T& on_data_get ( void ) final;
			virtual void on_data_fill ( void ) final;

			using LeanBase<T>::unset_data_created;

			T m_data;
		};

		template <typename T>
		Base<T>::Base() {
		}

		template <typename T>
		void Base<T>::on_data_fill() {
			this->on_data_create(m_data);
		}

		template <typename T>
		void Base<T>::clear_data() {
			this->unset_data_created();
			this->on_data_destroy(m_data);
		}

		template <typename T>
		T& Base<T>::on_data_get() {
			return m_data;
		}
	} //namespace scantask
} //namespace mchlib

#endif
