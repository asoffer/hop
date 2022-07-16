#ifndef JASMIN_EXECUTE_H
#define JASMIN_EXECUTE_H

#include <initializer_list>
#include <type_traits>

#include "jasmin/call_stack.h"
#include "jasmin/function.h"
#include "jasmin/instruction.h"
#include "jasmin/instruction_pointer.h"
#include "jasmin/internal/debug.h"
#include "jasmin/value.h"
#include "jasmin/value_stack.h"

namespace jasmin {

// Executes the given function `f` with an initial stack of values given by the
// object referenced by `value_stack`. `value_stack` is modified in place.
template <InstructionSet Set>
void Execute(Function<Set> const &f, ValueStack &value_stack) {
  CallStack call_stack;
  InstructionPointer ip = f.entry();
  call_stack.push(&f, value_stack.size(), ip);
  return Set::InstructionFunction(ip->op_code())(value_stack, ip, call_stack);
}

// Interprets the given function `f` with arguments provided in the
// initializer-list `arguments`. Return values are written to the passed-in
// references `return_values...`. In general, behavior is undefined if the
// number of return values of `f` is not `sizeof...(return_values)`, or if the
// values returned to not match the types
// `std::decay_t<decltype(return_values)...>`. However, if the macro
// `JASMIN_DEBUG` is defined then the behavior is defined to report an error
// message to `stderr` and abort program execution.
template <InstructionSet Set>
void Execute(Function<Set> const &f, std::initializer_list<Value> arguments,
             SmallTrivialValue auto &...return_values) {
  JASMIN_INTERNAL_DEBUG_ASSERT(arguments.size() == f.parameter_count(),
                               "Argument/parameter count mismatch");
  JASMIN_INTERNAL_DEBUG_ASSERT(sizeof...(return_values) == f.return_count(),
                               "Return value count mismatch");
  ValueStack value_stack(arguments);
  int dummy;

  Execute(f, value_stack);
  static_cast<void>(
      (dummy = ... =
           (return_values =
                value_stack.pop<std::decay_t<decltype(return_values)>>(),
            0)));
}

}  // namespace jasmin

#endif  // JASMIN_EXECUTE_H
