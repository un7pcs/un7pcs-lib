#pragma once
#include <cstddef>
#include <stddef.h>

namespace SimpleTL
{
// Null Type
struct NullType
{
};

// TypeList Base

template <typename... TYPES> struct TypeList;

template <typename HEAD, typename... TAIL> struct TypeList<HEAD, TAIL...>
{
    typedef HEAD Head;
    typedef TypeList<TAIL...> Tail;
};

template <typename HEAD> struct TypeList<HEAD>
{
    typedef HEAD Head;
    typedef NullType Tail;
};

template <> struct TypeList<>
{
    typedef NullType Head;
    typedef NullType Tail;
};

// TypeAt
template <typename List, size_t index> struct TypeAt
{
    typedef typename TypeAt<typename List::Tail, index - 1>::Result Result;
};

template <typename List> struct TypeAt<List, 0>
{
    typedef typename List::Head Result;
};

// IndexOf
template <typename List, typename T> struct IndexOf
{
  private:
    static constexpr size_t _temp = IndexOf<typename List::Tail, T>::Value;

  public:
    static constexpr size_t Value = (_temp == -1 ? -1 : 1 + _temp);
};

template <typename T> struct IndexOf<NullType, T>
{
    static constexpr size_t Value = -1;
};

template <typename T, typename... Tail> struct IndexOf<TypeList<T, Tail...>, T>
{
    static constexpr size_t Value = 0;
};

// Append
template <typename T1, typename T2> struct Append;

template <> struct Append<NullType, NullType>
{
    typedef NullType Result;
};

template <typename T> struct Append<NullType, T>
{
    typedef TypeList<T> Result;
};

template <typename T> struct Append<T, NullType>
{
    typedef TypeList<T> Result;
};

template <typename... LIST> struct Append<NullType, TypeList<LIST...>>
{
    typedef TypeList<LIST...> Result;
};

template <typename... LIST> struct Append<TypeList<LIST...>, NullType>
{
    typedef TypeList<LIST...> Result;
};

template <typename... LIST, typename T> struct Append<TypeList<LIST...>, T>
{
    typedef TypeList<LIST..., T> Result;
};

template <typename... LIST, typename T> struct Append<T, TypeList<LIST...>>
{
    typedef TypeList<T, LIST...> Result;
};

template <typename... LIST1, typename... LIST2> struct Append<TypeList<LIST1...>, TypeList<LIST2...>>
{
    typedef TypeList<LIST1..., LIST2...> Result;
};

// Erase
template <typename List, typename T> struct Erase;

template <typename T> // Specialization 1
struct Erase<NullType, T>
{
    typedef NullType Result;
};

template <typename... LIST> // Specialization 1.1
struct Erase<TypeList<LIST...>, NullType>
{
    typedef TypeList<LIST...> Result;
};

template <typename T, typename... Tail> // Specialization 2
struct Erase<TypeList<T, Tail...>, T>
{
    typedef TypeList<Tail...> Result;
};

template <typename... LIST, typename T> // Specialization 3
struct Erase<TypeList<LIST...>, T>
{
    typedef typename Append<typename TypeList<LIST...>::Head,
                            typename Erase<typename TypeList<LIST...>::Tail, T>::Result>::Result Result;
};

// EraseAll
template <typename List, typename T> struct EraseAll;

template <typename T> // Specialization 1
struct EraseAll<NullType, T>
{
    typedef NullType Result;
};

template <typename... LIST> // Specialization 1.1
struct EraseAll<TypeList<LIST...>, NullType>
{
    typedef TypeList<LIST...> Result;
};

template <typename T, typename... Tail> // Specialization 2
struct EraseAll<TypeList<T, Tail...>, T>
{
    typedef typename EraseAll<TypeList<Tail...>, T>::Result Result;
};

template <typename... LIST, typename T> // Specialization 3
struct EraseAll<TypeList<LIST...>, T>
{
    typedef typename Append<typename TypeList<LIST...>::Head,
                            typename EraseAll<typename TypeList<LIST...>::Tail, T>::Result>::Result Result;
};

// NoDuplicates

template <typename List> struct NoDuplicates;

template <> struct NoDuplicates<NullType>
{
    typedef NullType Result;
};

template <typename T> struct NoDuplicates<TypeList<T>>
{
    typedef TypeList<T> Result;
};

template <typename... LIST> struct NoDuplicates<TypeList<LIST...>>
{
  private:
    typedef typename NoDuplicates<typename TypeList<LIST...>::Tail>::Result L1;
    typedef typename Erase<L1, typename TypeList<LIST...>::Head>::Result L2;

  public:
    typedef typename Append<typename TypeList<LIST...>::Head, L2>::Result Result;
};

// Replace
template <typename List, typename T, typename U> struct Replace;

template <typename T, typename U> struct Replace<NullType, T, U>
{
    typedef NullType Result;
};

template <typename T, typename... TAIL, typename U> struct Replace<TypeList<T, TAIL...>, T, U>
{
    typedef TypeList<U, TAIL...> Result;
};

template <typename... LIST, typename T, typename U> struct Replace<TypeList<LIST...>, T, U>
{
    typedef typename Append<typename TypeList<LIST...>::Head,
                            typename Replace<typename TypeList<LIST...>::Tail, T, U>::Result>::Result Result;
};

// ReplaceAll
template <typename List, typename T, typename U> struct ReplaceAll;

template <typename T, typename U> struct ReplaceAll<NullType, T, U>
{
    typedef NullType Result;
};


template <typename T, typename... TAIL, typename U> struct ReplaceAll<TypeList<T, TAIL...>, T, U>
{
    typedef typename Append<U, typename ReplaceAll<TypeList<TAIL...>, T, U>::Result>::Result Result;
};

template <typename... LIST, typename T, typename U> struct ReplaceAll<TypeList<LIST...>, T, U>
{
    typedef typename Append<typename TypeList<LIST...>::Head,
                            typename ReplaceAll<typename TypeList<LIST...>::Tail, T, U>::Result>::Result Result;
};

class Test
{
  private:
    typedef TypeList<int, float, void, char> TL;

    constexpr static auto index = IndexOf<TL, void>::Value;

    typedef TypeAt<TL, index>::Result TA1;

    typedef Append<TL, TL>::Result A1;
    typedef Append<TL, double>::Result A2;
    typedef Append<double, TL>::Result A3;
    typedef Append<TL, NullType>::Result A4;
    typedef Append<NullType, TL>::Result A5;
    typedef Append<TL, TypeList<>>::Result A6;
    typedef Append<TypeList<>, TL>::Result A7;
    typedef Append<double, NullType>::Result A8;
    typedef Append<NullType, double>::Result A9;

    typedef Erase<A1, void>::Result ER1;
    typedef Erase<TL, int>::Result ER2;
    typedef EraseAll<A1, char>::Result ERA1;

    typedef NoDuplicates<ER1>::Result ND1;

    typedef Replace<TL, long, double>::Result RP1;
    typedef ReplaceAll<A1, void, char>::Result RP2;
};

}; // namespace SimpleTL
