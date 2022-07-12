#ifndef JASMIN_CALL_STACK_H
#define JASMIN_CALL_STACK_H

#include <vector>

#include "jasmin/internal/debug.h"
#include "jasmin/internal/function_base.h"

namespace jasmin {

// A `CallStack` represents the stack of functions called, enabling Jasmin's
// interpreter to track the location of the instruction pointer in previous
// function call frames.
struct CallStack {
  // Push the function `f` onto the call stack, tracking the existing stack size
  // as well as the location of the instruction pointer.
  void push(internal_function_base::FunctionBase const *f,
            size_t value_stack_size, InstructionPointer ip) {
    stack_.push_back({.function                     = f,
                      .stack_size_before_call       = value_stack_size,
                      .previous_instruction_pointer = ip});
  }

  // Returns whether the call stack is empty.
  constexpr bool empty() const { return stack_.empty(); }

  // Returns a pointer to the function currently at the top of the stack. During
  // execution, this is the function currently being executed.
  internal_function_base::FunctionBase const *current() const {
    JASMIN_INTERNAL_DEBUG_ASSERT(stack_.size() > 0,
                                 "Unexpectedly empty call stack");
    return stack_.back().function;
  }

  // Removes the top function from the call stack and returns the location of
  // the previous instruction pointer which was necessarily pointing to a
  // function call instruction.
  InstructionPointer pop() {
    InstructionPointer ip = stack_.back().previous_instruction_pointer;
    stack_.pop_back();
    return ip;
  }

  // TODO: This API is really gross and needs to be cleaned up.
  struct ErasableRange {
    size_t start_index;
    size_t end_index;
  };

  ErasableRange erasable_range(size_t current_value_stack_size) const {
    JASMIN_INTERNAL_DEBUG_ASSERT(stack_.size() > 0,
                                 "Unexpectedly empty call stack");
    auto [f, prev_size, ip] = stack_.back();
    JASMIN_INTERNAL_DEBUG_ASSERT(
        prev_size >= f->parameter_count(),
        "Previous stack size was unexpectedly less than the number of function "
        "parameters to the current function.");
    size_t prev_frame_end = prev_size - f->parameter_count();
    JASMIN_INTERNAL_DEBUG_ASSERT(prev_frame_end <= current_value_stack_size,
                                 "Current call stack frame clobbered values "
                                 "from previous call stack frame.");
    return {.start_index = prev_frame_end,
            .end_index   = current_value_stack_size - f->return_count()};
  }

 private:
  struct Frame {
    internal_function_base::FunctionBase const *function;
    size_t stack_size_before_call;
    InstructionPointer previous_instruction_pointer;
  };
  std::vector<Frame> stack_;
};

}  // namespace jasmin

#endif  // JASMIN_CALL_STACK_H
