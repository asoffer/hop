#ifndef JASMIN_INTERNAL_TYPE_LIST_H
#define JASMIN_INTERNAL_TYPE_LIST_H

#include <type_traits>

// Jasmin uses the type `void (*)(Ts*...)` to represent type-lists. This has the
// benefit over the slightly more common practice of `template <typename...>
// struct TypeList {}` of reducing template instantiations. Moreover, by using
// pointers to the types in question we are able to have incomplete types in our
// type lists.
namespace jasmin::internal {

template <typename... Ts>
using type_list = void (*)(Ts *...);

namespace internal_type_list {

// Implementation of `TypeList` concept, defined below.
template <typename>
struct IsTypeList : std::false_type {};
template <typename... Ts>
struct IsTypeList<type_list<Ts...>> : std::true_type {};

}  // namespace internal_type_list

// A concept identifying which types are type-lists
template <typename TL>
concept TypeList = internal_type_list::IsTypeList<TL>::value;

namespace internal_type_list {
// Implementation of `Concatenate`, defined below.
template <TypeList A, TypeList B>
struct ConcatenateImpl;
template <typename... As, typename... Bs>
struct ConcatenateImpl<type_list<As...>, type_list<Bs...>> {
  using type = type_list<As..., Bs...>;
};

// Implementation of `Apply`, defined below.
template <template <typename...> typename, typename>
struct ApplyImpl;
template <template <typename...> typename F, typename... Ts>
struct ApplyImpl<F, type_list<Ts...>> {
  using type = F<Ts...>;
};

}  // namespace internal_type_list

// Given two type-lists, evaluates to a type-list consisting of the
// concatenation of the two type-lists.
template <TypeList A, TypeList B>
using Concatenate = typename internal_type_list::ConcatenateImpl<A, B>::type;

// Given a type-function `F` and a type-list `type_list<Ts...>`, evaluates to
// the type `F<Ts...>`;
template <template <typename...> typename F, typename TypeList>
using Apply = typename internal_type_list::ApplyImpl<F, TypeList>::type;

}  // namespace jasmin::internal

#endif  // JASMIN_INTERNAL_TYPE_LIST_H
