#include <iostream>
#include <fstream>

template<long long n>
struct Fibonacchi {
    static const long long result = Fibonacchi<n - 1>::result + Fibonacchi<n - 2>::result;
};

template<>
struct Fibonacchi<0> {
    static const long long result = 1;
};

template<>
struct Fibonacchi<1> {
    static const long long result = 1;
};

//auto fib = Fibonacchi<90>::result;


//---------------------------------------------------------------------


template<class...>
struct TypeList;

template<>
struct TypeList<> {
    static const int Size = 0;
};

using EmptyTypeList = TypeList<>;

template <class H, class... T>
struct TypeList<H, T...> {
    using Head = H;
    using Tail = TypeList<T...>;

    static const int Size = sizeof...(T) + 1;
};

/*
template <int N, typename TypeList>
struct GetTypeListItem() {
    using Result = typename GetTypeListItem<N - 1, typename TypeList::Tail>::Result;
};

template <typename TypeList>
struct GetTypeListItem<0, TypeList> {
    using Result = typename TypeList::Head;
};
*/

//------------------------------------------------------------------


class Empty {};

template<template<class, class> class, typename H, typename...>
class GenLinearHierarchy;

template <template<class, class> class Unit, typename H, typename ...T>
class GenLinearHierarchy<Unit, TypeList<H, T ...>>:
    public Unit<H, GenLinearHierarchy<Unit, TypeList<T ...>>> {};

template<template<class, class> class Unit, typename H>
class GenLinearHierarchy<Unit, TypeList<H>> : public Unit<H, Empty> {};


//---------------------------------------------------------------------


template<template <class> class, class...>
class GenScatterHierarchy;

template <template <class> class Unit, class H, class ...T>
class GenScatterHierarchy<Unit, TypeList<H, T...>> :
    public GenScatterHierarchy<Unit, TypeList<H>>, 
    public GenScatterHierarchy<Unit, TypeList<T...>>
{
    using TList = TypeList<H, T...>;
    using LeftBase = GenScatterHierarchy<Unit, TypeList<H>>;
    using RightBase = GenScatterHierarchy<Unit, TypeList<T...>>;
};

template <template <class> class Unit, class AtomicType>
class GenScatterHierarchy<Unit, TypeList<AtomicType>> : public Unit<AtomicType>
{
    using LeftBase = Unit<AtomicType>;
};

template <template <class> class Unit>
class GenScatterHierarchy<Unit, EmptyTypeList> {};

//--------------------------------------------------------------------


template<int id_=0>
class Block {
	static const int id = id_;
};


template <class T, class Base>
class Unit_ : public Base {};

template<int n, typename ...>
class GenLinearTypeList;

template<int n, typename ...List>
class GenLinearTypeList {
public:
	using Result = typename GenLinearTypeList<n - 1, Block<n>, List...>::Result;
};

template<typename ...List>
class GenLinearTypeList<0, List...> {
public:
	using Result = TypeList<List...>;
};

template<int n, typename ...>
class GenFibTypeList;

template<int n, typename ...List>
class GenFibTypeList {
public:
	using LinearTypeList = typename GenLinearTypeList<Fibonacchi<n>::result>::Result;
	using Result = typename GenFibTypeList<n - 1, GenLinearHierarchy<Unit_, LinearTypeList>, List...>::Result;
};

template<typename ...List>
class GenFibTypeList <0, List...> {
public:
    using Result = TypeList<List...>;
};


//-------------------------------------------------------------------

template <class T>
class Holder {
	T value;
};


using LineTL = GenLinearTypeList<4>::Result;
using FibTL = GenFibTypeList<3>::Result;

GenLinearHierarchy<Unit_, LineTL> hierarchy1;
GenLinearHierarchy<Unit_, FibTL> hierarchy2;

GenScatterHierarchy<Holder, LineTL> hierarchy3;
GenScatterHierarchy<Holder, FibTL> hierarchy4;



int main() {
	//printf("%lld\n", fib);
}