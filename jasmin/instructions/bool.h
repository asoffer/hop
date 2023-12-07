#ifndef JASMIN_INSTRUCTIONS_BOOL_H
#define JASMIN_INSTRUCTIONS_BOOL_H

#include "jasmin/core/instruction.h"

namespace jasmin {

struct Not : Instruction<Not> {
  static bool consume(std::span<Value, 1> values) {
    return not values[0].as<bool>();
  }
  static constexpr std::string_view debug() { return "not"; }
};

struct Xor : Instruction<Xor> {
  static bool consume(std::span<Value, 2> values) {
    return values[0].as<bool>() xor values[1].as<bool>();
  }
  static constexpr std::string_view debug() { return "xor"; }
};

struct Or : Instruction<Or> {
  static bool consume(std::span<Value, 2> values) {
    return values[0].as<bool>() or values[1].as<bool>();
  }
  static constexpr std::string_view debug() { return "or"; }
};

struct And : Instruction<And> {
  static bool consume(std::span<Value, 2> values) {
    return values[0].as<bool>() and values[1].as<bool>();
  }
  static constexpr std::string_view debug() { return "and"; }
};

struct Nand : Instruction<Nand> {
  static bool consume(std::span<Value, 2> values) {
    return not(values[0].as<bool>() and values[1].as<bool>());
  }
  static constexpr std::string_view debug() { return "nand"; }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_BOOL_H
