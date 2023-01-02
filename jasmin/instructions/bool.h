#ifndef JASMIN_INSTRUCTIONS_BOOL_H
#define JASMIN_INSTRUCTIONS_BOOL_H

#include "jasmin/instruction.h"

namespace jasmin {

struct Not : StackMachineInstruction<Not> {
  static bool execute(bool value) { return not value; }
};

struct Xor : StackMachineInstruction<Xor> {
  static bool execute(bool x, bool y) { return x xor y; }
};

struct Or : StackMachineInstruction<Or> {
  static bool execute(bool x, bool y) { return x or y; }
};

struct And : StackMachineInstruction<And> {
  static bool execute(bool x, bool y) { return x and y; }
};

struct Nand : StackMachineInstruction<Nand> {
  static bool execute(bool x, bool y) { return not (x and y); }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_BOOL_H
