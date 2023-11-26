#ifndef JASMIN_INSTRUCTIONS_CORE_H
#define JASMIN_INSTRUCTIONS_CORE_H

#include "jasmin/instruction.h"

namespace jasmin {

struct Push : Instruction<Push> {
  static std::string_view name() { return "push"; }

  static constexpr Value execute(std::span<Value, 0>, Value v) { return v; }

  static std::string debug(std::span<Value const, 1> immediates) {
    return "push " + std::to_string(immediates[0].raw_value());
  }
};

struct Drop : Instruction<Drop> {
  static std::string_view name() { return "drop"; }
  static constexpr void execute(Value) {}
  static std::string debug(std::span<Value const, 0>) { return "drop"; }
};

struct Swap : Instruction<Swap> {
  static void execute(std::span<Value, 2> values) {
    std::swap(values[0], values[1]);
  }

  static constexpr std::string_view debug() { return "swap"; }
};

struct Duplicate : Instruction<Duplicate> {
  static std::string_view name() { return "duplicate"; }
  static Value execute(std::span<Value, 1> values) {
    return values[0]; }
  static constexpr std::string_view debug() { return "duplicate"; }
};

struct DuplicateAt : Instruction<DuplicateAt> {
  static std::string_view name() { return "duplicate-at"; }
  static Value execute(std::span<Value> values, size_t index) {
    return values[values.size() - 1 - index];
  }
  static std::string debug(std::span<Value const, 1> immediates) {
    return "duplicate @" + std::to_string(immediates[0].as<size_t>());
  }
};

struct Load : Instruction<Load> {
  static std::string_view debug() { return "load"; }
  static void consume(std::span<Value, 1> values, size_t size) {
    Value::Load(values[0].as<std::byte const *>(), size);
  }
};

struct Store : Instruction<Store> {
  static void consume(std::span<Value, 2> values, uint8_t size) {
    Value::Store(values[1], values[0].as<void *>(), size);
  }
  static constexpr std::string_view debug() { return "store"; }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_CORE_H
