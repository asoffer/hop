#ifndef JASMIN_INTERNAL_FUNCTION_STATE_H
#define JASMIN_INTERNAL_FUNCTION_STATE_H

#include "nth/meta/type.h"

namespace jasmin::internal {

// Concept matching instructions that have state associated with each function
// call. For such instructions Jasmin will create a stack of such states and
// automatically push/pop when functions are called/returned from respectively.
template <typename T>
concept HasFunctionState = requires {
  typename T::function_state;
};

// A list of all types required to represent the state of all functions in
// the instruction set `Set`.
template <typename Set>
constexpr auto FunctionStateList =
    Set::instructions
        .template transform<[](auto t) {
          using T = nth::type_t<t>;
          if constexpr (HasFunctionState<T>) {
            return nth::type<typename T::function_state>;
          } else {
            return nth::type<void>;
          }
        }>()
        .unique()
        .template filter<[](auto t) { return t != nth::type<void>; }>();

// A type-function accepting an instruction set and returning a type sufficient
// to hold all state required by all instructions in `Set`, or `void` if all
// instructions in `Set` are stateless.
template <typename Set>
using FunctionStateStack = nth::type_t<[](auto state_list) {
  if constexpr (state_list.empty()) {
    return nth::type<void>;
  } else {
    return state_list.reduce([](auto... ts) {
      return nth::type<std::stack<std::tuple<nth::type_t<ts>...>>>;
    });
  }
}(FunctionStateList<Set>)>;

}  // namespace jasmin::internal

#endif  // JASMIN_INTERNAL_FUNCTION_STATE_H
