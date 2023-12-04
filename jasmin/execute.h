#ifndef JASMIN_EXECUTE_H
#define JASMIN_EXECUTE_H

#include <initializer_list>
#include <type_traits>

#include "jasmin/function.h"
#include "jasmin/instruction.h"
#include "jasmin/value.h"
#include "nth/container/stack.h"
#include "nth/debug/debug.h"

namespace jasmin {

// Executes the given function `f` with an initial stack of values given by the
// object referenced by `value_stack`. `value_stack` is modified in place.
template <InstructionSetType Set>
void Execute(Function<Set> const &f, nth::stack<Value> &value_stack) {
  auto [top, remaining] = std::move(value_stack).release();
  Value head            = top;
  Value left            = remaining;

  using frame_type = internal::Frame<typename internal::FunctionState<Set>>;
  frame_type *call_stack =
      static_cast<frame_type *>(operator new(sizeof(frame_type) * 8));
  Value const *ip = f.entry();
  // These aren't actually instruction pointers, but we're using them during
  // execution to store information about the `nth::stack<Value>` so it can be
  // reconstituted after the fact.
  call_stack[0].ip = &head;
  call_stack[1].ip = &left;
  call_stack[2].ip = ip;

  ip->as<internal::exec_fn_type>()(top, remaining, ip, &call_stack[2],
                                   0x00000008'00000005);
  value_stack = nth::stack<Value>::reconstitute_from(head.as<Value *>(),
                                                     left.as<size_t>());
}

namespace internal {
inline Value Pop(nth::stack<Value> & stack) {
  Value v = stack.top();
  stack.pop();
  return v;
}
}  // namespace internal

// Interprets the given function `f` with arguments provided in the
// initializer-list `arguments`. Return values are written to the passed-in
// references `return_values...`. In general, behavior is undefined if the
// number of return values of `f` is not `sizeof...(return_values)`, or if the
// values returned to not match the types
// `std::decay_t<decltype(return_values)...>`. However, in debug and hardened
// builds (see `//jasmin/configuration` for details) the behavior is defined to
// report an error message to `stderr` and abort program execution.
template <InstructionSetType Set>
void Execute(Function<Set> const &f, std::initializer_list<Value> arguments,
             SmallTrivialValue auto &...return_values) {
  NTH_REQUIRE((v.when(internal::harden)),
              arguments.size() == f.parameter_count())
      .Log<"Argument/parameter count mismatch.">();
  NTH_REQUIRE((v.when(internal::harden)),
              sizeof...(return_values) == f.return_count())
      .Log<"Return value count mismatch.">();
  nth::stack<Value> value_stack(arguments);
  int dummy;

  Execute(f, value_stack);
  static_cast<void>(
      (dummy = ... =
           (return_values = internal::Pop(value_stack)
                                .as<std::decay_t<decltype(return_values)>>(),
            0)));
}

}  // namespace jasmin

#endif  // JASMIN_EXECUTE_H
