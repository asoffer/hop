#ifndef JASMIN_EXECUTE_H
#define JASMIN_EXECUTE_H

#include <initializer_list>
#include <type_traits>

#include "jasmin/call_stack.h"
#include "jasmin/function.h"
#include "jasmin/instruction.h"
#include "jasmin/instruction_pointer.h"
#include "jasmin/internal/type_traits.h"
#include "jasmin/value.h"
#include "jasmin/value_stack.h"
#include "nth/debug/debug.h"

namespace jasmin {

// Executes the given function `f` with an initial stack of values given by the
// object referenced by `value_stack`. `value_stack` is modified in place.
template <InstructionSet Set>
void Execute(Function<Set> const &f, ExecutionState<Set> exec_state,
             ValueStack &value_stack) {
  CallStack call_stack;
  InstructionPointer ip = f.entry();
  call_stack.push(&f, ip);
  using state_type = internal::State<Set>;
  state_type state{.exec_state = &exec_state};

  if constexpr (state_type::has_function_state) {
    state.function_state_stack.emplace();
  }

  using exec_fn_type =
      void (*)(ValueStack &, InstructionPointer &, CallStack &, state_type *);
  return ip->as<exec_fn_type>()(value_stack, ip, call_stack, &state);
}

template <InstructionSet Set>
requires(internal::ExecutionStateList<Set>.empty()) void Execute(
    Function<Set> const &f, ValueStack &value_stack) {
  ExecutionState<Set> state;
  Execute(f, state, value_stack);
}

// Interprets the given function `f` with arguments provided in the
// initializer-list `arguments`. Return values are written to the passed-in
// references `return_values...`. In general, behavior is undefined if the
// number of return values of `f` is not `sizeof...(return_values)`, or if the
// values returned to not match the types
// `std::decay_t<decltype(return_values)...>`. However, in debug and hardened
// builds (see `//jasmin/configuration` for details) the behavior is defined to
// report an error message to `stderr` and abort program execution.
template <InstructionSet Set>
void Execute(Function<Set> const &f, ExecutionState<Set> exec_state,
             std::initializer_list<Value> arguments,
             SmallTrivialValue auto &...return_values) {
  NTH_REQUIRE((v.when(internal::harden)),
              arguments.size() == f.parameter_count())
      .Log<"Argument/parameter count mismatch.">();
  NTH_REQUIRE((v.when(internal::harden)),
              sizeof...(return_values) == f.return_count())
      .Log<"Return value count mismatch.">();
  ValueStack value_stack(arguments);
  int dummy;

  Execute(f, exec_state, value_stack);
  static_cast<void>(
      (dummy = ... =
           (return_values =
                value_stack.pop<std::decay_t<decltype(return_values)>>(),
            0)));
}

template <InstructionSet Set>
requires(internal::ExecutionStateList<Set>.empty()) void Execute(
    Function<Set> const &f, std::initializer_list<Value> arguments,
    SmallTrivialValue auto &...return_values) {
  ExecutionState<Set> state;
  Execute(f, state, arguments, return_values...);
}

}  // namespace jasmin

#endif  // JASMIN_EXECUTE_H
