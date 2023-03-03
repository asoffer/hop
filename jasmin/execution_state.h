#ifndef JASMIN_EXECUTION_STATE_H
#define JASMIN_EXECUTION_STATE_H

#include <tuple>
#include <type_traits>

#include "jasmin/internal/type_traits.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace jasmin {

// Concept matching instruction types that have execution state.
template <typename T>
concept HasExecutionState = requires {
  typename T::execution_state;
};

namespace internal {

template <typename Set>
constexpr auto ExecutionStateList =
    Set::instructions
        .template transform<[](auto t) {
          using T = nth::type_t<t>;
          if constexpr (HasExecutionState<T>) {
            return nth::type<typename T::execution_state>;
          } else {
            return nth::type<void>;
          }
        }>()
        .unique()
        .template filter<[](auto t) { return t != nth::type<void>; }>();

template <typename Set>
using ExecutionState = nth::type_t<[](auto state_list) {
  if constexpr (state_list.empty()) {
    return nth::type<void>;
  } else {
    return state_list.reduce(
        [](auto... ts) { return nth::type<std::tuple<nth::type_t<ts> *...>>; });
  }
}(ExecutionStateList<Set>)>;

template <typename T>
struct ExecutionStateImpl {
  T execution_state_;
};

template <>
struct ExecutionStateImpl<void> {};

}  // namespace internal

// A struct holding all execution state for the given instruction set.
template <typename Set>
struct ExecutionState
    : private internal::ExecutionStateImpl<internal::ExecutionState<Set>> {
  // Requires that every state type used by the instruction set is initialized
  // with exactly one argument. Behavior is otherwise undefined.
  //
  // TODO: Validate requirement at compile-time.
  template <typename... Args>
  explicit ExecutionState(Args &...args) {
    ((std::get<Args *>(this->execution_state_) = std::addressof(args)), ...);
  }

  template <typename T>
  T const &get() const {
    return *std::get<T *>(this->execution_state_);
  }

  template <typename T>
  T &get() {
    return *std::get<T *>(this->execution_state_);
  }
};

}  // namespace jasmin

#endif  // JASMIN_EXECUTION_STATE_H
