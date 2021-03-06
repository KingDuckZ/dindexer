#ifndef safe_ptr_hh_INCLUDED
#define safe_ptr_hh_INCLUDED

// #define SAFE_PTR_TRACK_CALLSTACKS

#include "ref_ptr.hh"

#include <type_traits>
#include <utility>

#ifdef SAFE_PTR_TRACK_CALLSTACKS
#include "vector.hh"
#include <algorithm>
#endif

#include <cassert>

#define kak_assert assert

#if !defined(NDEBUG) && !defined(KAK_DEBUG)
#	define KAK_DEBUG
#endif

namespace Kakoune
{

// *** SafePtr: objects that assert nobody references them when they die ***

class SafeCountable
{
public:
#ifdef KAK_DEBUG
    SafeCountable() : m_count(0) {}
    ~SafeCountable()
    {
        kak_assert(m_count == 0);
        #ifdef SAFE_PTR_TRACK_CALLSTACKS
        kak_assert(m_callstacks.empty());
        #endif
    }

    friend void inc_ref_count(const SafeCountable* sc, void* /*ptr*/)
    {
        ++sc->m_count;
        #ifdef SAFE_PTR_TRACK_CALLSTACKS
        sc->m_callstacks.emplace_back(ptr);
        #endif
    }

    friend void dec_ref_count(const SafeCountable* sc, void* /*ptr*/)
    {
        --sc->m_count;
        kak_assert(sc->m_count >= 0);
        #ifdef SAFE_PTR_TRACK_CALLSTACKS
        auto it = std::find_if(sc->m_callstacks.begin(), sc->m_callstacks.end(),
                               [=](const Callstack& cs) { return cs.ptr == ptr; });
        kak_assert(it != sc->m_callstacks.end());
        sc->m_callstacks.erase(it);
        #endif
    }

    friend void ref_ptr_moved(const SafeCountable* /*sc*/, void* /*from*/, void* /*to*/)
    {
        #ifdef SAFE_PTR_TRACK_CALLSTACKS
        auto it = std::find_if(sc->m_callstacks.begin(), sc->m_callstacks.end(),
                               [=](const Callstack& cs) { return cs.ptr == from; });
        kak_assert(it != sc->m_callstacks.end());
        it->ptr = to;
        #endif
    }

private:
    #ifdef SAFE_PTR_TRACK_CALLSTACKS
    struct Callstack
    {
        Callstack(void* p) : ptr(p) {}
        void* ptr;
        Backtrace bt;
    };

    mutable Vector<Callstack> m_callstacks;
    #endif
    mutable int m_count;
#else
    [[gnu::always_inline]]
    friend void inc_ref_count(const SafeCountable* /*sc*/, void* /*ptr*/) {}

    [[gnu::always_inline]]
    friend void dec_ref_count(const SafeCountable* /*sc*/, void* /*ptr*/) {}
#endif
};

template<typename T, typename U>
using PropagateConst = typename std::conditional<std::is_const<T>::value, const U, U>::type;

template<typename T>
using SafePtr = RefPtr<T, PropagateConst<T, SafeCountable>>;

}

#if defined(KAK_DEBUG)
#	undef KAK_DEBUG
#endif
#undef kak_assert

#endif // safe_ptr_hh_INCLUDED
