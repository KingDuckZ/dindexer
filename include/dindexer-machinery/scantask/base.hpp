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

#include <ciso646>

namespace mchlib {
	namespace scantask {
		template <typename T>
		class Base {
		protected:
			Base ( void );
			virtual ~Base ( void ) noexcept = default;

		public:
			T& get_or_create ( void );
			void clear_data ( void );

		private:
			virtual void on_data_destroy ( T& parData ) = 0;
			virtual void on_data_create ( T& parData ) = 0;

			T m_data;
			bool m_data_created;
		};

		template <typename T>
		Base<T>::Base() :
			m_data_created(false)
		{
		}

		template <typename T>
		T& Base<T>::get_or_create() {
			if (not m_data_created) {
				m_data_created = true;
				this->on_data_create(m_data);
			}
			return m_data;
		}

		template <typename T>
		void Base<T>::clear_data() {
			if (m_data_created) {
				m_data_created = false;
				this->on_data_destroy(m_data);
			}
		}
	} //namespace scantask
} //namespace mchlib

#endif
