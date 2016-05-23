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

#ifndef idBC9F804ADD33468A9C7657E823FFC706
#define idBC9F804ADD33468A9C7657E823FFC706

#include "implem/AutomemBase.hpp"
#include "implem/IteratorOnPtr.hpp"
#include <memory>
#include <cstddef>
#include <type_traits>
#include <cassert>
#include <utility>

namespace dinhelp {
	const size_t MAXSZARR_MAX_STACK_ALLOC_SIZE = MAX_STACK_ALLOC_SIZE;

	//TODO: add a template parameter to force stack allocation regardless of
	//sizeof(T) * S
	template <typename T, size_t S, typename A=std::allocator<T> >
	class MaxSizedArray :
		private std::conditional<
			(sizeof(T) * S > (MAXSZARR_MAX_STACK_ALLOC_SIZE > 4 * sizeof(T*) ? MAXSZARR_MAX_STACK_ALLOC_SIZE - sizeof(T*) : MAXSZARR_MAX_STACK_ALLOC_SIZE)),
			AutomemRawBase_heap<T, S, A>,
			AutomemRawBase_stack<T, S>
		>::type
	{
		typedef typename std::conditional<
			(sizeof(T) * S > (MAXSZARR_MAX_STACK_ALLOC_SIZE > 4 * sizeof(T*) ? MAXSZARR_MAX_STACK_ALLOC_SIZE - sizeof(T*) : MAXSZARR_MAX_STACK_ALLOC_SIZE)),
			AutomemRawBase_heap<T, S, A>,
			AutomemRawBase_stack<T, S>
		>::type parent_type;

	public:
		typedef IteratorOnPtr<T*, T, false> iterator;
		typedef IteratorOnPtr<const T*, const T, false> const_iterator;
		typedef IteratorOnPtr<T*, T, true> reverse_iterator;
		typedef IteratorOnPtr<const T*, const T, true> const_reverse_iterator;
		typedef T& reference;
		typedef T&& mov_reference;
		typedef const T& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;

		enum MAX_SIZE_ENUM {
			MAX_SIZE = S
		};

		MaxSizedArray ( void );
		MaxSizedArray ( const MaxSizedArray& parOther );
		MaxSizedArray ( MaxSizedArray&& parOther );
		~MaxSizedArray ( void );

		bool empty ( void ) const;
		size_type size ( void ) const;
		size_type capacity ( void ) const { return S; }
		size_type max_size ( void ) const { return S; }
		void push_back ( const value_type& parNewItem );
		void push_back ( value_type&& parNewItem );
		void pop_back ( void );
		iterator erase ( const iterator& parDele );
		iterator erase ( const iterator& parFrom, const iterator& parToExcl );
		void clear ( void );
		mov_reference operator[] ( size_type parIndex );
		const_reference operator[] ( size_type parIndex ) const;
		MaxSizedArray& operator= ( const MaxSizedArray& parOther );
		bool operator== ( const MaxSizedArray& parOther ) const;
		bool operator!= ( const MaxSizedArray& parOther ) const;
		pointer GetPointer ( void );
		const_pointer GetPointer ( void ) const;
		void reserve ( size_type parReserve );

		iterator begin ( void );
		const_iterator begin ( void ) const;
		iterator end ( void );
		const_iterator end ( void ) const;
		reverse_iterator rbegin ( void );
		const_reverse_iterator rbegin ( void ) const;
		reverse_iterator rend ( void );
		const_reverse_iterator rend ( void ) const;
		reference front ( void ) { return (*this)[0]; }
		reference back ( void ) { return (*this)[size() - 1]; }
		const_reference front ( void ) const { return (*this)[0]; }
		const_reference back ( void ) const { return (*this)[size() - 1]; }

	private:
		pointer GetPointer_NoAssert ( void ) { return m_localMem; }
		const_pointer GetPointer_NoAssert ( void ) const { return m_localMem; }

		T* m_localMem; //A copy of memory pointer is required
		size_type m_used;
	};
} //namespace dinhelp

#include "MaxSizedArray.inl"

#endif
