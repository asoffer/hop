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
    JASMIN_INTERNAL_DEBUG_ASSERT(
        stack_.empty() or value_stack_size >= stack_.back().stack_size_before_call +
                                                  f->parameter_count(),
        "Value stack invariant broken: The size of the value-stack should "
        "always be non-decreasing");
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
    JASMIN_INTERNAL_DEBUG_ASSERT(stack_.size() > 0,
                                 "Unexpectedly empty call stack");
    InstructionPointer ip = stack_.back().previous_instruction_pointer;
    stack_.pop_back();
    return ip;
  }

  // Represents a range of values on a value stack that can be erased when the
  // top function from this call stack returns.
  struct ErasableRange {
    // The least-indexed element to be deleted. Indexing starts from the bottom
    // of the stack so this is the oldest element on the stack.
    size_t start_index;
    // The least-indexed element following a to-be-deleted range. In other
    // words, `end_index - 1` will be the newest element on the stack which is
    // to be deleted. Note: Another way to phrase this is that this range
    // represents the half-open range [start_index, end_index). If `start_index
    // == end_index`, the range is intended to be empty.
    size_t end_index;

    constexpr bool operator==(ErasableRange const &) const = default;
    constexpr bool operator!=(ErasableRange const &) const = default;
  };

  // Given the current size of the value stack, returns a range of elements to
  // be deleted as part the cleanup needed to return from the function at the
  // top of the call stack.
  ErasableRange erasable_range(size_t current_value_stack_size) const {
    JASMIN_INTERNAL_DEBUG_ASSERT(stack_.size() > 0,
                                 "Unexpectedly empty call stack");
    auto [f, prev_size, ip] = stack_.back();
    JASMIN_INTERNAL_DEBUG_ASSERT(
        prev_size >= f->parameter_count(),
        "Previous stack size was unexpectedly less than the number of function "
        "parameters to the current function.");
    size_t prev_frame_end = prev_size - f->parameter_count();
    JASMIN_INTERNAL_DEBUG_ASSERT(
        prev_frame_end + f->return_count() <= current_value_stack_size,
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
