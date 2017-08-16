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

		template <typename StrRef, typename Str>
		std::size_t start_pos (StrRef parSubstr, const Str* parData) {
			typedef decltype(parData->data()) char_type;
			assert(parData);

			if (not parSubstr.empty()) {
				assert(std::less_equal<char_type>()(parData->data(), parSubstr.data()));
				const std::size_t offset = parSubstr.data() - parData->data();
				return offset;
			}
			else {
				return 0;
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
	void StringPool<C, Str, StrRef>::update (ItR parDataBeg, ItR parDataEnd, const std::vector<const string_type*>& parBaseStrings) {
		typedef std::pair<string_type, std::size_t> PoolPair;

		while (parDataBeg != parDataEnd) {
			assert(parDataBeg->second < parBaseStrings.size());
			assert(parBaseStrings[parDataBeg->second] != nullptr);
			const auto* remote_source_str = parBaseStrings[parDataBeg->second];
			const StrRange& remote_str_rng = parDataBeg->first;
			const auto& remote_str_ref = stringref_type(*remote_source_str).substr(remote_str_rng.start, remote_str_rng.len);

			bool cloned = false;

			std::size_t idx = 0;
			for (auto& local_src : m_pool) {
				const string_type& local_str = local_src.first;
				auto& local_ref_count = local_src.second;

				auto cloned_result = implem::clone_ifp<StrRef>(remote_str_ref, local_str);
				cloned = cloned_result.second;
				const auto& cloned_str = cloned_result.first;
				if (cloned) {
					++local_ref_count;
					StrRange str_range {implem::start_pos(cloned_str, &local_str), cloned_str.size()};
					m_strings.push_back(StringListPair(str_range, idx));
					break;
				}
				++idx;
			}

			if (not cloned) {
				m_pool.push_back(PoolPair(*remote_source_str, static_cast<std::size_t>(1)));
				const std::size_t offset = implem::start_pos(remote_str_ref, remote_source_str);
				m_strings.push_back(
					StringListPair(
						StrRange{offset, remote_str_ref.size()},
						m_pool.size() - 1
					)
				);
			}
			++parDataBeg;
		}
	}

	template <typename C, typename Str, typename StrRef>
	void StringPool<C, Str, StrRef>::update (const StringPool& parOther) {
		std::vector<const string_type*> other_strs;
		other_strs.reserve(parOther.m_pool.size());
		for (auto& other_pool_itm : parOther.m_pool) {
			other_strs.push_back(&other_pool_itm.first);
		}
		update(parOther.m_strings.begin(), parOther.m_strings.end(), other_strs);
	}

	template <typename C, typename Str, typename StrRef>
	auto StringPool<C, Str, StrRef>::begin() const -> const_iterator {
		return const_iterator(m_strings.cbegin(), [this](const StringListPair& parItm) {
			return this->make_stringref(parItm);
		});
	}

	template <typename C, typename Str, typename StrRef>
	auto StringPool<C, Str, StrRef>::end() const -> const_iterator {
		return const_iterator(m_strings.cend(), [this](const StringListPair& parItm) {
			this->make_stringref(parItm);
		});
	}

	template <typename C, typename Str, typename StrRef>
	void StringPool<C, Str, StrRef>::insert (const std::vector<stringref_type>& parStrings, const string_type* parBaseString) {
		assert(parBaseString);

		StringListType dummy;
		dummy.reserve(parStrings.size());
		for (const auto& itm : parStrings) {
			StrRange str_range {implem::start_pos(itm, parBaseString), itm.size()};
			dummy.push_back(StringListPair(str_range, 0));
		}
		const std::vector<const string_type*> other_strs(1, parBaseString);
		update(dummy.begin(), dummy.end(), other_strs);
	}

	template <typename C, typename Str, typename StrRef>
	void StringPool<C, Str, StrRef>::insert (stringref_type parString, const string_type* parBaseString) {
		assert(parBaseString);
		assert(std::less_equal<const C*>()(parBaseString->data(), parString.data()));

		StringListType dummy;
		dummy.reserve(1);
		StrRange str_range {implem::start_pos(parString, parBaseString), parString.size()};
		dummy.push_back(StringListPair(str_range, 0));
		const std::vector<const string_type*> other_strs(1, parBaseString);
		update(dummy.begin(), dummy.end(), other_strs);
	}

	template <typename C, typename Str, typename StrRef>
	auto StringPool<C, Str, StrRef>::get_stringref_source (std::size_t parIndex) const -> const string_type* {
		return &m_pool[m_strings[parIndex].second].first;
	}

	template <typename C, typename Str, typename StrRef>
	auto StringPool<C, Str, StrRef>::operator[] (std::size_t parIndex) const -> stringref_type {
		return make_stringref(m_strings[parIndex]);
	}

	template <typename C, typename Str, typename StrRef>
	void StringPool<C, Str, StrRef>::pop() {
		if (m_strings.empty()) {
			return;
		}

		for (auto z = m_pool.size(); z > 0; --z) {
			auto& pool_itm = m_pool[z - 1];
			if (&pool_itm.first == &m_pool[m_strings.back().second].first) {
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

	template <typename C, typename Str, typename StrRef>
	auto StringPool<C, Str, StrRef>::make_stringref (const StringListPair& parStrPair) const -> stringref_type  {
		assert(parStrPair.second < m_pool.size());
		return stringref_type(m_pool[parStrPair.second].first).substr(parStrPair.first.start, parStrPair.first.len);
	}
} //namespace mchlib
