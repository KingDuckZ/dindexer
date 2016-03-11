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

#ifndef id904376BAD85D4782B83CAAEC2EF344D9
#define id904376BAD85D4782B83CAAEC2EF344D9

#include "dindexer-machinery/scantask/leanbase.hpp"
#include <memory>
#include <vector>
#include <utility>
#include <ciso646>

namespace mchlib {
	namespace scantask {
		template <typename T>
		class GeneralFiller : public LeanBase<T> {
		public:
			using FillingTaskPtr = std::shared_ptr<LeanBase<T>>;

			template <typename P, typename... PP>
			GeneralFiller ( std::shared_ptr<P> parFill, std::shared_ptr<PP>... parFillList );
			virtual ~GeneralFiller ( void ) noexcept;

		private:
			virtual void on_data_fill ( void ) override;
			virtual T& on_data_get ( void ) override;

			std::vector<FillingTaskPtr> m_to_fill;
		};

		template <typename T>
		template <typename P, typename... PP>
		GeneralFiller<T>::GeneralFiller (std::shared_ptr<P> parFill, std::shared_ptr<PP>... parFillList) :
			m_to_fill { std::move(parFill), std::move(parFillList)... }
		{
		}

		template <typename T>
		GeneralFiller<T>::~GeneralFiller() noexcept {
			m_to_fill.clear();
		}

		template <typename T>
		void GeneralFiller<T>::on_data_fill() {
			for (auto& itm : m_to_fill) {
				itm->get_or_create();
			}
		}

		template <typename T>
		T& GeneralFiller<T>::on_data_get() {
			assert(not m_to_fill.empty());
			return m_to_fill.front()->get_or_create();
		}
	} //namespace scantask
} //namespace mchlib

#endif
