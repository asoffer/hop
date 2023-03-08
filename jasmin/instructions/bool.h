#ifndef JASMIN_INSTRUCTIONS_BOOL_H
#define JASMIN_INSTRUCTIONS_BOOL_H

#include "jasmin/instruction.h"

namespace jasmin {

struct Not : StackMachineInstruction<Not> {
  static bool execute(bool value) { return not value; }
  static constexpr std::string_view debug() { return "not"; }
};

struct Xor : StackMachineInstruction<Xor> {
  static bool execute(bool x, bool y) { return x xor y; }
  static constexpr std::string_view debug() { return "xor"; }
};

struct Or : StackMachineInstruction<Or> {
  static bool execute(bool x, bool y) { return x or y; }
  static constexpr std::string_view debug() { return "or"; }
};

struct And : StackMachineInstruction<And> {
  static bool execute(bool x, bool y) { return x and y; }
  static constexpr std::string_view debug() { return "and"; }
};

struct Nand : StackMachineInstruction<Nand> {
  static bool execute(bool x, bool y) { return not (x and y); }
  static constexpr std::string_view debug() { return "nand"; }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_BOOL_H
