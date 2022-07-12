#ifndef JASMIN_INSTRUCTIONS_CORE_H
#define JASMIN_INSTRUCTIONS_CORE_H

#include <cstring>

#include "jasmin/execute.h"

namespace jasmin {

struct Push : StackMachineInstruction<Push> {
  static constexpr void execute(ValueStack &value_stack,
                                InstructionPointer &ip) {
    ++ip;
    value_stack.push(ip->value());
    ++ip;
  }
};

struct Duplicate : StackMachineInstruction<Duplicate> {
  static constexpr void execute(ValueStack &value_stack,
                                InstructionPointer &ip) {
    value_stack.push(value_stack.peek_value());
    ++ip;
  }
};

struct Swap : StackMachineInstruction<Swap> {
  static constexpr void execute(ValueStack &value_stack,
                                InstructionPointer &ip) {
    value_stack.swap_with(1);
    ++ip;
  }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_CORE_H
