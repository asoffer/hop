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
namespace internal {

inline Value Pop(nth::stack<Value> &stack) {
  Value v = stack.top();
  stack.pop();
  return v;
}

template <typename StateType>
void FinishExecution(Value *value_stack_head, size_t vs_left, Value const *ip,
                     FrameBase *call_stack, uint64_t cs_remaining) {
  *(ip + 1)->as<Value **>() = value_stack_head;
  *(ip + 2)->as<size_t *>() = vs_left;
  nth::stack<Frame<StateType>>::reconstitute_from(
      static_cast<Frame<StateType> *>(call_stack), cs_remaining);
}

}  // namespace internal

// Executes the given function `f` with an initial stack of values given by the
// object referenced by `value_stack`. `value_stack` is modified in place.
template <InstructionSetType Set>
void Execute(Function<Set> const &f, nth::stack<Value> &value_stack) {
  using frame_type = internal::Frame<internal::FunctionState<Set>>;
  nth::stack<frame_type> call_stack;
  call_stack.emplace();

  auto [top, remaining] = std::move(value_stack).release();
  Value landing_pad[5]  = {
       Value::Uninitialized(), Value::Uninitialized(),
       &internal::FinishExecution<internal::FunctionState<Set>>, &top,
       &remaining};
  call_stack.top().ip = &landing_pad[0];

  Value const *ip             = f.entry();
  auto [cs_top, cs_remaining] = std::move(call_stack).release();
  ip->as<internal::exec_fn_type>()(top, remaining, ip, cs_top, cs_remaining);
  value_stack = nth::stack<Value>::reconstitute_from(top, remaining);
}

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
