#ifndef JASMIN_INTERNAL_TYPE_LIST_H
#define JASMIN_INTERNAL_TYPE_LIST_H

#include <type_traits>

#include "jasmin/internal/type_traits.h"

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

// Implementation of `Transform`, defined below.
template <template <typename...> typename, typename>
struct TransformImpl;
template <template <typename...> typename F, typename... Ts>
struct TransformImpl<F, type_list<Ts...>> {
  using type = type_list<F<Ts>...>;
};

// Implementation of `Unique`, defined below.
template <typename, typename>
struct UniqueImpl;
template <typename T, typename... Ts, typename... Results>
requires(std::is_same_v<T, Results> or ...)  //
    struct UniqueImpl<type_list<T, Ts...>, type_list<Results...>>
    : UniqueImpl<type_list<Ts...>, type_list<Results...>> {
};
template <typename T, typename... Ts, typename... Results>
requires((not std::is_same_v<T, Results>)and...)  //
    struct UniqueImpl<type_list<T, Ts...>, type_list<Results...>>
    : UniqueImpl<type_list<Ts...>, type_list<T, Results...>> {
};
template <typename... Results>
struct UniqueImpl<type_list<>, type_list<Results...>> {
  using type = type_list<Results...>;
};

template <auto, TypeList>
struct InvokeImpl;

template <auto Lambda, typename... Ts>
struct InvokeImpl<Lambda, type_list<Ts...>> {
  static constexpr auto value = Lambda.template operator()<Ts...>();
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

// Given a type-function `F` and a type-list `type_list<Ts...>`, evaluates to
// the type-list `type_list<F<Ts>...>`;
template <template <typename...> typename F, typename TypeList>
using Transform = typename internal_type_list::TransformImpl<F, TypeList>::type;

// Given a type-list, evaluates to a new type-list in which
// * No element of the result is repeated
// * Each element of the resulting type-list is contained in the input
//   type-list.
// * Each element in the original type-list is contained in the resulting
//   type-list.
// No guarantee is provided on the ordering of the elements in the resulting
// type-list.
template <typename TypeList>
using Unique =
    typename internal_type_list::UniqueImpl<TypeList, type_list<>>::type;

// Given an invocable `lambda` that can be invoked by passing a bunch of
// template arguments, invokes `lambda` with the arguments from the given type
// list `L`.
template <auto Lambda, TypeList L>
inline constexpr auto Invoke = internal_type_list::InvokeImpl<Lambda, L>::value;

// A concept indicating that the matching type is contained in the given type
// list.
template <typename T, typename L>
concept ContainedIn = TypeList<L> and
    Invoke<[]<typename... Ts>() { return (std::is_same_v<T, Ts> or ...); }, L>;

}  // namespace jasmin::internal

#endif  // JASMIN_INTERNAL_TYPE_LIST_H
