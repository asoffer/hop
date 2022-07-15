#ifndef JASMIN_INSTRUCTIONS_CORE_H
#define JASMIN_INSTRUCTIONS_CORE_H

#include "jasmin/instruction.h"

namespace jasmin {

struct Push : StackMachineInstruction<Push> {
  static constexpr void execute(ValueStack &value_stack, Value v) {
    value_stack.push(v);
  }
};

struct Duplicate : StackMachineInstruction<Duplicate> {
  static constexpr void execute(ValueStack &value_stack) {
    value_stack.push(value_stack.peek_value());
  }
};

struct Swap : StackMachineInstruction<Swap> {
  static constexpr void execute(ValueStack &value_stack) {
    value_stack.swap_with(1);
  }
};

struct JumpIf : StackMachineInstruction<JumpIf> {
  static constexpr ptrdiff_t execute(ValueStack &value_stack, ptrdiff_t n) {
    return value_stack.pop<bool>() ? n : 2;
  }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_CORE_H
