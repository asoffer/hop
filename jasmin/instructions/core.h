#ifndef JASMIN_INSTRUCTIONS_CORE_H
#define JASMIN_INSTRUCTIONS_CORE_H

#include "jasmin/instruction.h"

namespace jasmin {

struct Push : StackMachineInstruction<Push> {
  static std::string_view name() { return "push"; }

  static constexpr ValueStackRef execute(ValueStackRef value_stack, Value v) {
    return ValueStackRef::Push(std::move(value_stack), v);
  }
};

struct Drop : StackMachineInstruction<Drop> {
  static std::string_view name() { return "drop"; }

  static constexpr ValueStackRef execute(ValueStackRef value_stack,
                                         size_t count) {
    return ValueStackRef::EraseLast(std::move(value_stack), count);
  }
  static std::string debug(std::span<Value const, 1> immediates) {
    return "drop " + std::to_string(immediates[0].as<size_t>());
  }
};

struct Duplicate : StackMachineInstruction<Duplicate> {
  static std::string_view name() { return "duplicate"; }

  static ValueStackRef execute(ValueStackRef value_stack) {
    Value v = value_stack.peek_value();
    return ValueStackRef::Push(std::move(value_stack), v);
  }
  static constexpr std::string_view debug() { return "duplicate"; }
};

struct DuplicateAt : StackMachineInstruction<DuplicateAt> {
  static std::string_view name() { return "duplicate-at"; }

  static ValueStackRef execute(ValueStackRef value_stack, size_t index) {
    Value v = value_stack.peek_value(index);
    return ValueStackRef::Push(std::move(value_stack), v);
  }
  static std::string debug(std::span<Value const, 1> immediates) {
    return "duplicate @" + std::to_string(immediates[0].as<size_t>());
  }
};

struct Swap : StackMachineInstruction<Swap> {
  static ValueStackRef execute(ValueStackRef value_stack) {
    value_stack.swap_with(1);
    return value_stack;
  }

  static constexpr std::string_view debug() { return "swap"; }
};

struct Load : StackMachineInstruction<Load> {
  static ValueStackRef execute(ValueStackRef value_stack, uint8_t size) {
    Value v = Value::Load(value_stack.pop<std::byte const *>(), size);
    return ValueStackRef::Push(std::move(value_stack), v);
  }
  static constexpr std::string_view debug() { return "load"; }
};

struct Store : StackMachineInstruction<Store> {
  static ValueStackRef execute(ValueStackRef value_stack, uint8_t size) {
    Value value    = value_stack.pop_value();
    void *location = value_stack.pop<void *>();
    Value::Store(value, location, size);
    return value_stack;
  }
  static constexpr std::string_view debug() { return "store"; }
};

struct StoreConstant : StackMachineInstruction<StoreConstant> {
  static ValueStackRef execute(ValueStackRef value_stack, void *location,
                               uint8_t size) {
    Value::Store(value_stack.pop_value(), location, size);
    return value_stack;
  }
  static constexpr std::string_view debug() { return "store-constant"; }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_CORE_H
