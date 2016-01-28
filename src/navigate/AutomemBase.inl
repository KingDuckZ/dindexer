/* Copyright 2016, Michele Santullo
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

namespace din {
///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
template <typename T, size_t S, typename A>
AutomemBase_heap<T, S, A>::AutomemBase_heap() {
#if !defined(NDEBUG)
	m_localMem = NULL;
#endif
}

///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
template <typename T, size_t S, typename A>
AutomemBase_heap<T, S, A>::AutomemBase_heap (const AutomemBase_heap& parOther) {
#if !defined(NDEBUG)
	m_localMem = NULL;
#endif
	AllocMemory();
	std::copy(parOther.GetMemPtr(), parOther.GetMemPtr() + S, this->GetMemPtr());
}

///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
template <typename T, size_t S, typename A>
AutomemBase_heap<T, S, A>::AutomemBase_heap (AutomemBase_heap&& parOther) {
	std::swap(parOther.m_localMem, this->m_localMem);
}

///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
template <typename T, size_t S, typename A>
void AutomemBase_heap<T, S, A>::AllocMemory() {
#if !defined(NDEBUG)
	Assert(NULL == m_localMem);
#endif
	m_localMem = A().allocate(S);
	for (size_t z = 0; z < S; ++z) {
		new(m_localMem + z) T;
	}
}

///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
template <typename T, size_t S, typename A>
void AutomemBase_heap<T, S, A>::FreeMemory() noexcept {
#if !defined(NDEBUG)
	Assert(NULL != m_localMem);
#endif
	for (size_t z = 0; z < S; ++z) {
		m_localMem->~T();
	}
	A().deallocate(m_localMem, S);
#if !defined(NDEBUG)
	m_localMem = NULL;
#endif
}

///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
template <typename T, size_t S>
AutomemBase_stack<T, S>::AutomemBase_stack() {
}

///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
template <typename T, size_t S>
AutomemBase_stack<T, S>::AutomemBase_stack (const AutomemBase_stack& parOther) {
	std::copy(parOther.GetMemPtr(), parOther.GetMemPtr() + S, this->GetMemPtr());
}

///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
template <typename T, size_t S>
AutomemBase_stack<T, S>::AutomemBase_stack (AutomemBase_stack&& parOther) {
	for (size_t z = 0; z < S; ++z) {
		std::swap(m_localMem[z], parOther.m_localMem[z]);
	}
}

///-------------------------------------------------------------------------
///-------------------------------------------------------------------------
template <typename T, size_t S, typename A>
AutomemRawBase_heap<T, S, A>::AutomemRawBase_heap() {
#if !defined(NDEBUG)
	m_localMem = NULL;
#endif
}

///-------------------------------------------------------------------------
///-------------------------------------------------------------------------
template <typename T, size_t S, typename A>
void AutomemRawBase_heap<T, S, A>::AllocMemory() {
#if !defined(NDEBUG)
	Assert(NULL == m_localMem);
#endif
	m_localMem = A().allocate(S);
#if defined(ASSERTIONSENABLED)
	assert(reinterpret_cast<PTR_INT_TYPE>(m_localMem) % alignof(T) == 0); //Make sure alignment is correct
#endif
}

///-------------------------------------------------------------------------
///-------------------------------------------------------------------------
template <typename T, size_t S, typename A>
void AutomemRawBase_heap<T, S, A>::FreeMemory() noexcept {
#if !defined(NDEBUG)
	Assert(NULL != m_localMem);
#endif
	A().deallocate(m_localMem, S);
#if !defined(NDEBUG)
	m_localMem = NULL;
#endif
}

///-------------------------------------------------------------------------
///-------------------------------------------------------------------------
template <typename T, size_t S, typename A>
char* AutomemRawBase_heap<T, S, A>::GetMemPtrAtIndex (size_t parIndex) {
	T* const retVal = GetMemPtr() + parIndex;
	return reinterpret_cast<char*>(retVal);
}

///-------------------------------------------------------------------------
///-------------------------------------------------------------------------
template <typename T, size_t S>
void AutomemRawBase_stack<T, S>::AllocMemory() {
#if defined(ASSERTIONSENABLED)
	assert(reinterpret_cast<PTR_INT_TYPE>(m_localMem) % alignof(T) == 0); //Make sure alignment is correct
#endif
}

///-------------------------------------------------------------------------
///-------------------------------------------------------------------------
template <typename T, size_t S>
char* AutomemRawBase_stack<T, S>::GetMemPtrAtIndex (size_t parIndex) {
	T* const retVal = GetMemPtr() + parIndex;
#if defined(ASSERTIONSENABLED)
	assert(reinterpret_cast<PTR_INT_TYPE>(retVal) % alignof(T) == 0);
#endif
	return reinterpret_cast<char*>(retVal);
}
} //namespace din
