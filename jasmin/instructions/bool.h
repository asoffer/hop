#ifndef JASMIN_INSTRUCTIONS_BOOL_H
#define JASMIN_INSTRUCTIONS_BOOL_H

#include "jasmin/core/instruction.h"

namespace jasmin {

struct Not : Instruction<Not> {
  static void execute(std::span<Value, 1> values, std::span<Value, 0>) {
    values[0] = not values[0].as<bool>();
  }
};

struct Xor : Instruction<Xor> {
  static void consume(std::span<Value, 2> values, std::span<Value, 1> out) {
    out[0] = static_cast<bool>(values[0].as<bool>() xor values[1].as<bool>());
  }
};

struct Or : Instruction<Or> {
  static void consume(std::span<Value, 2> values, std::span<Value, 1> out) {
    out[0] = values[0].as<bool>() or values[1].as<bool>();
  }
};

struct And : Instruction<And> {
  static void consume(std::span<Value, 2> values, std::span<Value, 1> out) {
    out[0] = values[0].as<bool>() and values[1].as<bool>();
  }
};

struct Nand : Instruction<Nand> {
  static void consume(std::span<Value, 2> values, std::span<Value, 1> out) {
    out[0] = not(values[0].as<bool>() and values[1].as<bool>());
  }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_BOOL_H
