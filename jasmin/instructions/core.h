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

struct Load : StackMachineInstruction<Load> {
  static void execute(ValueStack &value_stack, uint8_t size) {
    std::byte const *p = value_stack.pop<std::byte const *>();
    value_stack.push(Value::Load(p, size));
  }
};

struct Store : StackMachineInstruction<Store> {
  static void execute(ValueStack &value_stack, uint8_t size) {
    Value value    = value_stack.pop_value();
    void *location = value_stack.pop<void *>();
    Value::Store(value, location, size);
  }
};

struct StoreConstant : StackMachineInstruction<StoreConstant> {
  static void execute(ValueStack &value_stack, void *location, uint8_t size) {
    Value::Store(value_stack.pop_value(), location, size);
  }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_CORE_H
