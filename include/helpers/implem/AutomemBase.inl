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
	namespace {
#if defined(ASSERTIONSENABLED)
		const char g_guard = 0xAB;
#endif
	} //unnamed namespace

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	AutomemRawBase_heap<T, S, A>::AutomemRawBase_heap() {
#if !defined(NDEBUG)
		m_localMem = nullptr;
#endif
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	AutomemRawBase_heap<T, S, A>::AutomemRawBase_heap (AutomemRawBase_heap&& parOther) {
#if !defined(NDEBUG)
		m_localMem = nullptr;
#endif
		this->swap(parOther);
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	T* AutomemRawBase_heap<T, S, A>::AllocMemory() {
#if !defined(NDEBUG)
		Assert(nullptr == m_localMem);
#endif
		m_localMem = A().allocate(S);
#if defined(ASSERTIONSENABLED)
		assert(reinterpret_cast<PTR_INT_TYPE>(m_localMem) % alignof(T) == 0); //Make sure alignment is correct
		std::fill(
			reinterpret_cast<char*>(&m_localMem[0]),
			reinterpret_cast<char*>(&m_localMem[0]) + sizeof(m_localMem),
			g_guard
		);
#endif
		return m_localMem;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	void AutomemRawBase_heap<T, S, A>::FreeMemory() noexcept {
#if !defined(NDEBUG)
		Assert(nullptr != m_localMem);
#endif
		A().deallocate(m_localMem, S);
#if !defined(NDEBUG)
		m_localMem = nullptr;
#endif
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A>
	template <typename... Args>
	T* AutomemRawBase_heap<T, S, A>::GetNewT (size_t parIndex, Args&&... parArgs) {
		assert(parIndex < S);
		T* const location = m_localMem + parIndex;
#if defined(ASSERTIONSENABLED)
		assert(reinterpret_cast<PTR_INT_TYPE>(location) % alignof(T) == 0);
		assert(g_guard == *reinterpret_cast<const char*>(location));
#endif
		return new(location) T(std::forward<Args...>(parArgs)...);
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S>
	T* AutomemRawBase_stack<T, S>::AllocMemory() {
#if defined(ASSERTIONSENABLED)
		assert(reinterpret_cast<PTR_INT_TYPE>(m_localMem) % alignof(T) == 0); //Make sure alignment is correct
		std::fill(
			reinterpret_cast<char*>(&m_localMem[0]),
			reinterpret_cast<char*>(&m_localMem[0]) + sizeof(m_localMem),
			g_guard
		);
#endif
		return reinterpret_cast<T*>(&m_localMem[0]);
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename T, size_t S>
	template <typename... Args>
	T* AutomemRawBase_stack<T, S>::GetNewT (size_t parIndex, Args&&... parArgs) {
		assert(parIndex < S);
		auto* const location = &m_localMem[parIndex];
#if defined(ASSERTIONSENABLED)
		assert(reinterpret_cast<PTR_INT_TYPE>(location) % alignof(T) == 0);
		assert(g_guard == *reinterpret_cast<const char*>(location));
#endif
		return new(location) T(std::forward<Args...>(parArgs)...);
	}
} //namespace dinhelp
