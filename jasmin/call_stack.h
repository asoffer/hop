#ifndef JASMIN_CALL_STACK_H
#define JASMIN_CALL_STACK_H

#include <vector>

#include "jasmin/internal/function_base.h"
#include "nth/debug/debug.h"

namespace jasmin {

// A `CallStack` represents the stack of functions called, enabling Jasmin's
// interpreter to track the location of the instruction pointer in previous
// function call frames.
struct CallStack {
  // Push the function `f` onto the call stack, tracking the existing stack size
  // as well as the location of the instruction pointer.
  void push(internal::FunctionBase const *f, Value const *ip) {
    stack_.push_back({.function                     = f,
                      .previous_instruction_pointer = ip});
  }

  // Returns whether the call stack is empty.
  constexpr bool empty() const { return stack_.empty(); }

  // Returns a pointer to the function currently at the top of the stack. During
  // execution, this is the function currently being executed.
  internal::FunctionBase const *current() const {
    NTH_REQUIRE((v.when(internal::harden)), stack_.size() > size_t{0})
        .Log<"Unexpectedly empty call stack.">();
    return stack_.back().function;
  }

  // Removes the top function from the call stack and returns the location of
  // the previous instruction pointer which was necessarily pointing to a
  // function call instruction.
  Value const *pop() {
    NTH_REQUIRE((v.when(internal::harden)), stack_.size() > size_t{0})
        .Log<"Unexpectedly empty call stack.">();
    Value const *ip = stack_.back().previous_instruction_pointer;
    stack_.pop_back();
    return ip;
  }

 private:
  struct Frame {
    internal::FunctionBase const *function;
    Value const *previous_instruction_pointer;
  };
  std::vector<Frame> stack_;
};

}  // namespace jasmin

#endif  // JASMIN_CALL_STACK_H
