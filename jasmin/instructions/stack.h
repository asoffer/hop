#ifndef JASMIN_INSTRUCTIONS_STACK_H
#define JASMIN_INSTRUCTIONS_STACK_H

#include <cstddef>
#include <memory>

#include "jasmin/core/instruction.h"
#include "jasmin/core/value.h"
#include "nth/debug/debug.h"

namespace jasmin {
namespace internal {

struct StackFrame {
  std::byte *allocate_once(size_t size_in_bytes) {
    NTH_REQUIRE(data() == nullptr);
    data_.reset(new std::byte[size_in_bytes]);
    return data_.get();
  }

  std::byte const *data() const { return data_.get(); }
  std::byte *data() { return data_.get(); }

 private:
  std::unique_ptr<std::byte[]> data_;
};

}  // namespace internal

// Initializes sufficient space in the stack frame. Must be called before any
// other stack-related instructions on the current `jasmin::Function`. Must not
// be called more than once on any `jasmin::Function`.
struct StackAllocate : Instruction<StackAllocate> {
  using function_state = internal::StackFrame;
  static constexpr void execute(function_state &frame, Input<>, Output<>,
                                size_t size_in_bytes) {
    frame.allocate_once(size_in_bytes);
  }
};

// Returns a pointer into the stack frame associated with the current function,
// offset by the amount `offset`.
struct StackOffset : Instruction<StackOffset> {
  using function_state = internal::StackFrame;
  static constexpr void execute(function_state &frame, Input<>,
                                Output<std::byte *> out, size_t offset) {
    out.set<0>(frame.data() + offset);
  }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_STACK_H
