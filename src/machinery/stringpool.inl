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

namespace mchlib {
	namespace implem {
		template <typename StrRef>
		std::pair<StrRef, bool> clone_ifp (const StrRef& parClone, StrRef parSource) {
			const auto offset = parSource.find(parClone);
			if (parSource.npos != offset) {
				return std::make_pair(parSource.substr(offset, parClone.size()), true);
			}
			else {
				return std::make_pair(parClone, false);
			}
		}
	} //namespace implem

	template <typename C, typename Str, typename StrRef>
	auto StringPool<C, Str, StrRef>::ptr_to_literal (const char* parLiteral) -> const string_type* {
		if (not parLiteral)
			return nullptr;

		for (const auto& p : m_pool) {
			if (m_pool.first == parLiteral) {
				return &m_pool.first;
			}
		}
		return nullptr;
	}

	template <typename C, typename Str, typename StrRef>
	template <typename ItR>
	void StringPool<C, Str, StrRef>::update (ItR parDataBeg, ItR parDataEnd) {
		typedef std::pair<string_type, std::size_t> PoolPair;

		while (parDataBeg != parDataEnd) {
			const auto& remote_str = parDataBeg->first;
			const auto* remote_source_str = parDataBeg->second;
			bool cloned = false;

			for (auto& local_src : m_pool) {
				const string_type& local_str = local_src.first;
				auto& local_ref_count = local_src.second;

				auto cloned_result = implem::clone_ifp<StrRef>(remote_str, local_str);
				cloned = cloned_result.second;
				const auto& cloned_str = cloned_result.first;
				if (cloned) {
					++local_ref_count;
					m_strings.push_back(StringListPair(cloned_str, &local_str));
					break;
				}
			}

			if (not cloned) {
				m_pool.push_back(PoolPair(*remote_source_str, static_cast<std::size_t>(1)));
				const auto offset = remote_str.data() - remote_source_str->data();
				m_strings.push_back(StringListPair(stringref_type(m_pool.back().first).substr(offset, remote_str.size()), &m_pool.back().first));
			}
			++parDataBeg;
		}
	}

	template <typename C, typename Str, typename StrRef>
	void StringPool<C, Str, StrRef>::update (const StringPool& parOther) {
		this->update(parOther.m_strings.begin(), parOther.m_strings.end());
	}

	template <typename C, typename Str, typename StrRef>
	auto StringPool<C, Str, StrRef>::begin() const -> const_iterator {
		return const_iterator(m_strings.cbegin(), [](const StringListPair& parItm) { return parItm.first; });
	}

	template <typename C, typename Str, typename StrRef>
	auto StringPool<C, Str, StrRef>::end() const -> const_iterator {
		return const_iterator(m_strings.cend(), [](const StringListPair& parItm) { return parItm.first; });
	}

	template <typename C, typename Str, typename StrRef>
	void StringPool<C, Str, StrRef>::insert (const std::vector<stringref_type>& parStrings, const string_type* parBaseString) {
		StringListType dummy;
		dummy.reserve(parStrings.size());
		for (const auto& itm : parStrings) {
			dummy.push_back(StringListPair(itm, parBaseString));
		}
		this->update(dummy.begin(), dummy.end());
	}

	template <typename C, typename Str, typename StrRef>
	void StringPool<C, Str, StrRef>::insert (stringref_type parString, const string_type* parBaseString) {
		StringListType dummy;
		dummy.reserve(1);
		dummy.push_back(StringListPair(parString, parBaseString));
		this->update(dummy.begin(), dummy.end());
	}

	template <typename C, typename Str, typename StrRef>
	auto StringPool<C, Str, StrRef>::get_stringref_source (std::size_t parIndex) const -> const string_type* {
		return m_strings[parIndex].second;
	}

	template <typename C, typename Str, typename StrRef>
	auto StringPool<C, Str, StrRef>::operator[] (std::size_t parIndex) const -> const stringref_type& {
		return m_strings[parIndex].first;
	}

	template <typename C, typename Str, typename StrRef>
	void StringPool<C, Str, StrRef>::pop() {
		if (m_strings.empty()) {
			return;
		}

		for (auto z = m_pool.size(); z > 0; --z) {
			auto& pool_itm = m_pool[z - 1];
			if (&pool_itm.first == m_strings.back().second) {
				m_strings.resize(m_strings.size() - 1);
				--pool_itm.second;
				if (0 == pool_itm.second) {
					m_pool.erase(m_pool.begin() + (z - 1));
				}
				break;
			}
		}
		return;
	}

	template <typename C, typename Str, typename StrRef>
	void StringPool<C, Str, StrRef>::swap (StringPool& parOther) noexcept {
		m_pool.swap(parOther.m_pool);
		m_strings.swap(parOther.m_strings);
	}
} //namespace mchlib
