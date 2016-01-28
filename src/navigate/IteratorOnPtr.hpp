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

#ifndef id1B822B1775424D879AA6AD1739B5BC91
#define id1B822B1775424D879AA6AD1739B5BC91

#include <iterator>
#include <cstdint>
#include <type_traits>
#include <cassert>

namespace din {
	namespace implem_iop {
		template <typename P, typename T, int32_t I>
		class IteratorOnPtr_base : public std::iterator<std::random_access_iterator_tag, T> {
			typedef std::iterator<std::random_access_iterator_tag, T> parent_type;
		public:
			//Typedefs to be STL-compliant
			typedef typename parent_type::iterator_category iterator_category;
			typedef typename parent_type::value_type value_type;
			typedef typename parent_type::difference_type difference_type;
			typedef typename parent_type::pointer pointer;
			typedef typename std::remove_reference<T>::type& reference;

			IteratorOnPtr_base ( P parPointer, difference_type parSize );
			IteratorOnPtr_base ( const IteratorOnPtr_base& parOther );
			template <typename P1, typename T1>
			explicit IteratorOnPtr_base ( const IteratorOnPtr_base<P1, T1, I>& parOther );
			~IteratorOnPtr_base ( void );

			difference_type operator- ( const IteratorOnPtr_base& parOther ) const;

			bool operator== ( const IteratorOnPtr_base& parOther ) const { return m_pointer == parOther.m_pointer; }
			bool operator!= ( const IteratorOnPtr_base& parOther ) const { return not operator==(parOther); }
			bool operator< ( const IteratorOnPtr_base& parOther ) const { return m_pointer < parOther.m_pointer; }
			bool operator>= ( const IteratorOnPtr_base& parOther ) const { return not operator<(parOther); }
			bool operator> ( const IteratorOnPtr_base& parOther ) const { return parOther < *this; }
			bool operator<= ( const IteratorOnPtr_base& parOther ) const { return not operator>(parOther); }

			P GetPointer ( void ) { return m_pointer; }
			const P GetPointer ( void ) const { return m_pointer; }
#if !defined(NDEBUG)
			difference_type GetSize ( void ) const { return GetSize(0); }
#endif

		protected:
			enum {
				STEP = (I < 0 ? -I : I)
			};

#if !defined(NDEBUG)
			difference_type GetSize ( difference_type parAdvance ) const;
			bool CanAdvance ( difference_type parAdvance ) const;
			void AlterSize ( difference_type parAdvance );
#endif
			void MoveIterator ( difference_type parAdvance );

			P m_pointer;

		private:
#if !defined(NDEBUG)
			difference_type m_size;
#endif
		};
	} //namespace implem_iop

	template <typename P, typename T, bool R>
	class IteratorOnPtr : public implem_iop::IteratorOnPtr_base<P, T, (R ? -1 : 1)> {
		typedef implem_iop::IteratorOnPtr_base<P, T, (R ? -1 : 1)> parent_type;
		enum {
			STEP = parent_type::STEP
		};
	public:
		typedef typename parent_type::iterator_category iterator_category;
		typedef typename parent_type::value_type value_type;
		typedef typename parent_type::difference_type difference_type;
		typedef typename parent_type::pointer pointer;
		typedef typename parent_type::reference reference;

		IteratorOnPtr ( void );
		IteratorOnPtr ( P parPointer, difference_type parSize );
		IteratorOnPtr ( const IteratorOnPtr& parOther ) : parent_type(parOther) { return; }
		template <typename P1, typename T1>
		IteratorOnPtr ( const IteratorOnPtr<P1, T1, R>& parOther ) : parent_type(parOther) {}
		~IteratorOnPtr ( void );

		IteratorOnPtr& operator++ ( void ); //pre
		IteratorOnPtr operator++ ( int ); //post
		IteratorOnPtr& operator-- ( void );
		IteratorOnPtr operator-- ( int );
		reference operator* ( void );
		pointer operator-> ( void );

		using parent_type::operator-;
		IteratorOnPtr operator+ ( difference_type parOther ) const;
		IteratorOnPtr operator- ( difference_type parOther ) const;
		IteratorOnPtr& operator+= ( difference_type parOther );
		IteratorOnPtr& operator-= ( difference_type parOther );
	protected:
	private:
	};

	namespace implem_iop {
		///---------------------------------------------------------------------
		///---------------------------------------------------------------------
		template <typename P, typename T, int32_t I>
#if !defined(NDEBUG)
		IteratorOnPtr_base<P, T, I>::IteratorOnPtr_base (P parPointer, difference_type parSize) :
			m_pointer(parPointer),
			m_size(parSize)
#else
		IteratorOnPtr_base<P, T, I>::IteratorOnPtr_base (P parPointer, difference_type) :
			m_pointer(parPointer)
#endif
		{
			static_assert(I != 0, "Step must be non-zero");
		}

		///---------------------------------------------------------------------
		///---------------------------------------------------------------------
		template <typename P, typename T, int32_t I>
		template <typename P1, typename T1>
		IteratorOnPtr_base<P, T, I>::IteratorOnPtr_base (const IteratorOnPtr_base<P1, T1, I>& parOther) {
			m_pointer = parOther.GetPointer();
#if !defined(NDEBUG)
			m_size = parOther.GetSize();
#endif
		}

		///---------------------------------------------------------------------
		///---------------------------------------------------------------------
		template <typename P, typename T, int32_t I>
		IteratorOnPtr_base<P, T, I>::IteratorOnPtr_base (const IteratorOnPtr_base& parOther) {
			m_pointer = parOther.m_pointer;
#if !defined(NDEBUG)
			m_size = parOther.m_size;
#endif
		}

		///---------------------------------------------------------------------
		///---------------------------------------------------------------------
		template <typename P, typename T, int32_t I>
		IteratorOnPtr_base<P, T, I>::~IteratorOnPtr_base() {
		}

		///---------------------------------------------------------------------
		///---------------------------------------------------------------------
		template <typename P, typename T, int32_t I>
		typename IteratorOnPtr_base<P, T, I>::difference_type IteratorOnPtr_base<P, T, I>::operator- (const IteratorOnPtr_base& parOther) const {
			if (I > 0)
				return m_pointer - parOther.m_pointer;
			else
				return parOther.m_pointer - m_pointer;
		}

#if !defined(NDEBUG)
		///---------------------------------------------------------------------
		///---------------------------------------------------------------------
		template <typename P, typename T, int32_t I>
		typename IteratorOnPtr_base<P, T, I>::difference_type IteratorOnPtr_base<P, T, I>::GetSize (difference_type parAdvance) const {
			return m_size - STEP * parAdvance;
		}

		///---------------------------------------------------------------------
		///---------------------------------------------------------------------
		template <typename P, typename T, int32_t I>
		bool IteratorOnPtr_base<P, T, I>::CanAdvance (difference_type parAdvance) const {
			return (m_size >= STEP * parAdvance);
		}

		///---------------------------------------------------------------------
		///---------------------------------------------------------------------
		template <typename P, typename T, int32_t I>
		void IteratorOnPtr_base<P, T, I>::AlterSize (difference_type parAdvance) {
			m_size = GetSize(parAdvance);
		}
#endif

		///---------------------------------------------------------------------
		///---------------------------------------------------------------------
		template <typename P, typename T, int32_t I>
		void IteratorOnPtr_base<P, T, I>::MoveIterator (difference_type parAdvance) {
#if !defined(NDEBUG)
			assert(CanAdvance(parAdvance));
			AlterSize(parAdvance);
#endif
			m_pointer += I * parAdvance;
		}
	} //namespace implem_iop

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename P, typename T, bool R>
	IteratorOnPtr<P, T, R>::IteratorOnPtr() :
		parent_type(NULL, 0)
	{
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename P, typename T, bool R>
	IteratorOnPtr<P, T, R>::IteratorOnPtr (P parPointer, difference_type parSize) :
		parent_type(parPointer, parSize)
	{
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename P, typename T, bool R>
	IteratorOnPtr<P, T, R>::~IteratorOnPtr() {
	}

	///-------------------------------------------------------------------------
	///Pre-increment.
	///-------------------------------------------------------------------------
	template <typename P, typename T, bool R>
	IteratorOnPtr<P, T, R>& IteratorOnPtr<P, T, R>::operator++() {
		this->MoveIterator(1);
		return *this;
	}

	///-------------------------------------------------------------------------
	///Post-increment.
	///-------------------------------------------------------------------------
	template <typename P, typename T, bool R>
	IteratorOnPtr<P, T, R> IteratorOnPtr<P, T, R>::operator++ (int) {
		IteratorOnPtr<P, T, R> retVal(*this);
		this->MoveIterator(1);
		return retVal;
	}

	///-------------------------------------------------------------------------
	///Pre-decrement.
	///-------------------------------------------------------------------------
	template <typename P, typename T, bool R>
	IteratorOnPtr<P, T, R>& IteratorOnPtr<P, T, R>::operator--() {
		this->MoveIterator(-1);
		return *this;
	}

	///-------------------------------------------------------------------------
	///Post-decrement.
	///-------------------------------------------------------------------------
	template <typename P, typename T, bool R>
	IteratorOnPtr<P, T, R> IteratorOnPtr<P, T, R>::operator-- (int) {
		IteratorOnPtr<P, T, R> retVal(*this);
		this->MoveIterator(-1);
		return retVal;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename P, typename T, bool R>
	typename IteratorOnPtr<P, T, R>::reference IteratorOnPtr<P, T, R>::operator*() {
		return *(this->m_pointer);
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename P, typename T, bool R>
	typename IteratorOnPtr<P, T, R>::pointer IteratorOnPtr<P, T, R>::operator->() {
		return this->m_pointer;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename P, typename T, bool R>
	IteratorOnPtr<P, T, R> IteratorOnPtr<P, T, R>::operator+ (difference_type parOther) const {
		IteratorOnPtr<P, T, R> retVal(*this);
		retVal += parOther;
		return retVal;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename P, typename T, bool R>
	IteratorOnPtr<P, T, R> IteratorOnPtr<P, T, R>::operator- (difference_type parOther) const {
		IteratorOnPtr<P, T, R> retVal(*this);
		retVal -= parOther;
		return retVal;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename P, typename T, bool R>
	IteratorOnPtr<P, T, R>& IteratorOnPtr<P, T, R>::operator+= (difference_type parOther) {
		this->MoveIterator(parOther);
		return *this;
	}

	///-------------------------------------------------------------------------
	///-------------------------------------------------------------------------
	template <typename P, typename T, bool R>
	IteratorOnPtr<P, T, R>& IteratorOnPtr<P, T, R>::operator-= (difference_type parOther) {
		this->MoveIterator(-parOther);
		return *this;
	}
} //namespace din

#endif
