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

namespace dinhelp {
	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	MaxSizedArray<T, S, A>::MaxSizedArray() {
		m_localMem = this->AllocMemory();
		m_used = 0;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	MaxSizedArray<T, S, A>::MaxSizedArray (const MaxSizedArray& parOther) :
		parent_type(),
		m_used(0)
	{
		m_localMem = this->AllocMemory();
		const size_type count = parOther.size();
		for (size_type z = 0; z < count; ++z) {
			this->push_back(parOther[z]);
		}
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	MaxSizedArray<T, S, A>::MaxSizedArray (MaxSizedArray&& parOther) :
		parent_type(),
		m_used(0)
	{
		m_localMem = this->AllocMemory();
		const size_type count = parOther.size();
		for (size_type z = 0; z < count; ++z) {
			this->push_back(std::move(parOther[z]));
		}
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	MaxSizedArray<T, S, A>::~MaxSizedArray() {
		this->clear();
		this->FreeMemory();
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	bool MaxSizedArray<T, S, A>::empty() const {
		return 0 == m_used;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::size_type MaxSizedArray<T, S, A>::size() const {
		return m_used;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	void MaxSizedArray<T, S, A>::push_back (value_type&& parNewItem) {
		assert(size() < capacity());
		this->GetNewT(m_used, std::move(parNewItem));
		++m_used;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	void MaxSizedArray<T, S, A>::push_back (const value_type& parNewItem) {
		assert(size() < capacity());
		this->GetNewT(m_used, parNewItem);
		++m_used;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	void MaxSizedArray<T, S, A>::pop_back() {
		assert(not empty());
		m_localMem[m_used - 1].~T();
		--m_used;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::reference MaxSizedArray<T, S, A>::operator[] (size_type parIndex) & {
		assert(parIndex < size());
		return m_localMem[parIndex];
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::mov_reference MaxSizedArray<T, S, A>::operator[] (size_type parIndex) && {
		assert(parIndex < size());
		return std::move(m_localMem[parIndex]);
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::const_reference MaxSizedArray<T, S, A>::operator[] (size_type parIndex) const & {
		assert(parIndex < size());
		return m_localMem[parIndex];
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	MaxSizedArray<T, S, A>& MaxSizedArray<T, S, A>::operator= (const MaxSizedArray& parOther) {
		m_used = parOther.m_used;
		std::copy(parOther.GetMemPtr(), parOther.GetMemPtr() + parOther.size(), this->GetMemPtr());
		return *this;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	bool MaxSizedArray<T, S, A>::operator== (const MaxSizedArray& parOther) const {
		if (size() != parOther.size())
			return false;
		for (size_type z = 0; z < size(); ++z) {
			if ((*this)[z] != parOther[z])
				return false;
		}
		return true;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	bool MaxSizedArray<T, S, A>::operator!= (const MaxSizedArray& parOther) const {
		return not (*this == parOther);
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::pointer MaxSizedArray<T, S, A>::GetPointer() {
		assert(size() > 0);
		return GetPointer_NoAssert();
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::const_pointer MaxSizedArray<T, S, A>::GetPointer() const {
		assert(size() > 0);
		return GetPointer_NoAssert();
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	void MaxSizedArray<T, S, A>::clear() {
		const size_type count = this->size();
		for (size_type z = 0; z < count; ++z) {
			(*this)[z].~T();
		}
		m_used = 0;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::iterator MaxSizedArray<T, S, A>::begin() {
		return iterator(GetPointer_NoAssert(), size());
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::const_iterator MaxSizedArray<T, S, A>::begin() const {
		return const_iterator(GetPointer_NoAssert(), size());
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::iterator MaxSizedArray<T, S, A>::end() {
		return iterator(GetPointer_NoAssert() + size(), 0);
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::const_iterator MaxSizedArray<T, S, A>::end() const {
		return const_iterator(GetPointer_NoAssert() + size(), 0);
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::reverse_iterator MaxSizedArray<T, S, A>::rbegin() {
		return reverse_iterator(GetPointer_NoAssert() + size() - 1, size());
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::const_reverse_iterator MaxSizedArray<T, S, A>::rbegin() const {
		return const_reverse_iterator(GetPointer_NoAssert() + size() - 1, size());
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::reverse_iterator MaxSizedArray<T, S, A>::rend() {
		return reverse_iterator(GetPointer_NoAssert() - 1, 0);
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::const_reverse_iterator MaxSizedArray<T, S, A>::rend() const {
		return const_reverse_iterator(GetPointer_NoAssert() - 1, 0);
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::iterator MaxSizedArray<T, S, A>::erase (const iterator& parDele) {
		assert(end() != parDele);
		return erase(parDele, parDele + 1);
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	typename MaxSizedArray<T, S, A>::iterator MaxSizedArray<T, S, A>::erase (const iterator& parFrom, const iterator& parToExcl) {
		assert(parFrom >= begin());
		assert(parToExcl <= end());
		assert(parToExcl >= parFrom);

		//I'm doing this in two passes: first, I'm deleting as many elements as
		//the ones that would be left at the end (that is, delete and move),
		//then delete everything to the end of the buffer, if necessary.
		const size_type deleCount = static_cast<size_type>(parToExcl - parFrom);
		const size_type firstIndexToDele = static_cast<size_type>(parFrom - begin());
		const size_type& sz = m_used;
		assert(firstIndexToDele + deleCount <= sz);
		const size_type deleAndCopyCount = sz - (firstIndexToDele + deleCount);

		//As said, make room and copy from the cut tail
		for (size_type z = firstIndexToDele; z < firstIndexToDele + deleAndCopyCount; z++) {
			(*this)[z].~T();
			new(&(*this)[z]) T((*this)[z + deleCount]);
		}

		//Any leftover is rubbish
		for (size_type z = firstIndexToDele + deleAndCopyCount; z < sz; z++) {
			(*this)[z].~T();
		}

		m_used -= deleCount;
		return begin() + firstIndexToDele;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	void MaxSizedArray<T, S, A>::reserve (size_type parReserve) {
		assert(parReserve <= S);
		if (parReserve > S) {
			throw std::length_error("Unable to reserve more memory than the build-time size for MaxSizedArray");
		}
	}
} //namespace dinhelp
