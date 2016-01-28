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

#ifndef idDB299AE3079F4830BB1D543C07F7B594
#define idDB299AE3079F4830BB1D543C07F7B594

//Classes in this file are low level and only intended to be used by library
//code. They don't do much by themselves and shoud never be used from outside.

#include <memory>
#include <algorithm>
#include <cassert>
#include <cstdint>

#if !defined(NDEBUG)
#	define ASSERTIONSENABLED
#endif

namespace din {
	const size_t MAX_STACK_ALLOC_SIZE = 128;

	///-------------------------------------------------------------------------
	///Heap-based allocation, only gets raw memory
	///-------------------------------------------------------------------------
	template <typename T, size_t S, typename A=std::allocator<T> >
	class AutomemRawBase_heap {
	protected:
		AutomemRawBase_heap ( void );
		AutomemRawBase_heap ( const AutomemRawBase_heap& ) = delete; //Copy ctor can't be implemented at this level
		AutomemRawBase_heap ( AutomemRawBase_heap&& parOther );

		template <typename... Args>
		T* GetNewT ( size_t parIndex, Args&&... parArgs );
		T* AllocMemory ( void );
		void FreeMemory ( void ) noexcept;
		void swap ( AutomemRawBase_heap& parOther ) noexcept { std::swap(m_localMem, parOther.m_localMem); }
	private:
		void operator= (const AutomemRawBase_heap&);

#if defined(ASSERTIONSENABLED)
		typedef uintptr_t PTR_INT_TYPE;
		static_assert(sizeof(PTR_INT_TYPE) == sizeof(char*), "Wrong uintptr_t size");
#endif
		T* m_localMem;
	};

	///-------------------------------------------------------------------------
	///Stack-based allocation, only gets raw memory
	///-------------------------------------------------------------------------
	template <typename T, size_t S>
	class AutomemRawBase_stack {
	protected:
		AutomemRawBase_stack ( void ) = default;
		AutomemRawBase_stack ( const AutomemRawBase_stack& ) = delete; //Copy ctor can't be implemented at this level
		AutomemRawBase_stack ( AutomemRawBase_stack&& ) = delete;
		~AutomemRawBase_stack ( void ) = default;

		template <typename... Args>
		T* GetNewT ( size_t parIndex, Args&&... parArgs );
		T* AllocMemory ( void );
		void FreeMemory ( void ) noexcept { return; }
	private:
#if defined(ASSERTIONSENABLED)
		typedef uintptr_t PTR_INT_TYPE;
		static_assert(sizeof(PTR_INT_TYPE) == sizeof(char*), "Wrong uintptr_t size");
#endif
		void operator= (const AutomemRawBase_stack&);
		typename std::aligned_storage<sizeof(T), alignof(T)>::type m_localMem[S];
	};
} //namespace din

#include "AutomemBase.inl"

#if defined(ASSERTIONSENABLED)
#	undef ASSERTIONSENABLED
#endif
#endif
