#include "kaballoc/trait/relocatable.h"

#define CAT_2(A, B) A, ## B

struct A {};
namespace foo { struct B { }; }
template<typename T1> struct C {};
template<typename T1, typename T2> struct D {};
namespace bar { template<typename T1, typename T2, typename T3> struct E{}; }

KAB_DECLARE_RELOCATABLE(A);
KAB_DECLARE_RELOCATABLE(foo::B);
KAB_DECLARE_RELOCATABLE_TMP_1(C);
KAB_DECLARE_RELOCATABLE_TMP_2(D);
KAB_DECLARE_RELOCATABLE_TMP_3(bar::E);

static_assert(kab::is_trivially_relocatable_v<A>);
static_assert(kab::is_trivially_relocatable_v<foo::B>);
static_assert(kab::is_trivially_relocatable_v<C<int>>);
static_assert(kab::is_trivially_relocatable_v<D<int, int>>);
static_assert(kab::is_trivially_relocatable_v<bar::E<int, float, double>>);

struct A2 {};
namespace foo { struct B2 { }; }
template<typename T1> struct C2 {};
template<typename T1, typename T2> struct D2 {};
namespace bar { template<typename T1, typename T2, typename T3> struct E2 {}; }

KAB_DECLARE_RELOCATABLE_COND(A2, true);
KAB_DECLARE_RELOCATABLE_COND(foo::B2, is_trivially_relocatable_v<A2>);
KAB_DECLARE_RELOCATABLE_COND_TMP_1(C2, is_trivially_relocatable_v<T1>);
KAB_DECLARE_RELOCATABLE_COND_TMP_2(D2, is_trivially_relocatable_v<C2<int>>);
KAB_DECLARE_RELOCATABLE_COND_TMP_3(bar::E2, CAT_2(is_trivially_relocatable_v<D2<int, T2>>));

static_assert(kab::is_trivially_relocatable_v<A2>);
static_assert(kab::is_trivially_relocatable_v<foo::B2>);
static_assert(kab::is_trivially_relocatable_v<C2<int>>);
static_assert(kab::is_trivially_relocatable_v<D2<int, int>>);
static_assert(kab::is_trivially_relocatable_v<bar::E2<int, float, double>>);