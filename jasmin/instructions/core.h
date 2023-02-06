#ifndef JASMIN_INSTRUCTIONS_CORE_H
#define JASMIN_INSTRUCTIONS_CORE_H

#include "jasmin/instruction.h"

namespace jasmin {

struct Push : StackMachineInstruction<Push> {
  static std::string_view name() { return "push"; }

  static constexpr void execute(ValueStack &value_stack, Value v) {
    value_stack.push(v);
  }
};

struct Drop : StackMachineInstruction<Drop> {
  static std::string_view name() { return "drop"; }

  static constexpr void execute(ValueStack &value_stack, size_t count) {
    value_stack.erase(value_stack.size() - count, value_stack.size());
  }
};

struct Duplicate : StackMachineInstruction<Duplicate> {
  static std::string_view name() { return "duplicate"; }

  static constexpr void execute(ValueStack &value_stack) {
    value_stack.push(value_stack.peek_value());
  }
};

struct DuplicateAt : StackMachineInstruction<DuplicateAt> {
  static std::string_view name() { return "duplicate-at"; }

  static constexpr void execute(ValueStack &value_stack, size_t index) {
    value_stack.push(value_stack.peek_value(index));
  }
};

struct Swap : StackMachineInstruction<Swap> {
  static std::string_view name() { return "swap"; }

  static constexpr void execute(ValueStack &value_stack) {
    value_stack.swap_with(1);
  }
};

struct Load : StackMachineInstruction<Load> {
  static std::string_view name() { return "load"; }

  static void execute(ValueStack &value_stack, uint8_t size) {
    std::byte const *p = value_stack.pop<std::byte const *>();
    value_stack.push(Value::Load(p, size));
  }
};

struct Store : StackMachineInstruction<Store> {
  static std::string_view name() { return "store"; }

  static void execute(ValueStack &value_stack, uint8_t size) {
    Value value    = value_stack.pop_value();
    void *location = value_stack.pop<void *>();
    Value::Store(value, location, size);
  }
};

struct StoreConstant : StackMachineInstruction<StoreConstant> {
  static std::string_view name() { return "store-constant"; }

  static void execute(ValueStack &value_stack, void *location, uint8_t size) {
    Value::Store(value_stack.pop_value(), location, size);
  }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_CORE_H
