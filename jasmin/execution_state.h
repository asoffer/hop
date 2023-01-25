#ifndef JASMIN_EXECUTION_STATE_H
#define JASMIN_EXECUTION_STATE_H

#include <tuple>
#include <type_traits>

#include "jasmin/internal/type_list.h"
#include "jasmin/internal/type_traits.h"

namespace jasmin {

// Concept matching instruction types that have execution state.
template <typename T>
concept HasExecutionState = requires {
  typename T::JasminExecutionState;
};

namespace internal {

template <typename T>
struct NotVoid {
  static constexpr bool value = not std::is_void_v<T>;
};

template <typename Set>
using ExecutionStateList = FromNth<
    ToNth(std::type_identity_t<typename Set::jasmin_instructions *>{})
        .template transform<[](auto t) {
          using T = nth::type_t<t>;
          if constexpr (HasExecutionState<T>) {
            return nth::type<typename T::JasminExecutionState>;
          } else {
            return nth::type<void>;
          }
        }>()
        .unique()
        .template filter<[](auto t) { return t != nth::type<void>; }>()>;

template <typename>
struct ExecutionStateImpl;

template <>
struct ExecutionStateImpl<void (*)()> {};

template <typename... States>
struct ExecutionStateImpl<void (*)(States *...)> {
  std::tuple<States*...> execution_state_;
};

}  // namespace internal

// A struct holding all execution state for the given instruction set.
template <typename Set>
struct ExecutionState
    : private internal::ExecutionStateImpl<internal::ExecutionStateList<Set>> {
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
