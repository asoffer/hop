#ifndef JASMIN_INSTRUCTIONS_BOOL_H
#define JASMIN_INSTRUCTIONS_BOOL_H

#include "hop/core/instruction.h"

namespace hop {

struct Not : Instruction<Not> {
  static void consume(Input<bool> in, Output<bool> out) {
    out.set<0>(not in.get<0>());
  }
};

struct Xor : Instruction<Xor> {
  static void consume(Input<bool, bool> in, Output<bool> out) {
    out.set<0>(in.get<0>() xor in.get<1>());
  }
};

struct Or : Instruction<Or> {
  static void consume(Input<bool, bool> in, Output<bool> out) {
    out.set<0>(in.get<0>() or in.get<1>());
  }
};

struct And : Instruction<And> {
  static void consume(Input<bool, bool> in, Output<bool> out) {
    out.set<0>(in.get<0>() and in.get<1>());
  }
};

struct Nand : Instruction<Nand> {
  static void consume(Input<bool, bool> in, Output<bool> out) {
    out.set<0>(not(in.get<0>() and in.get<1>()));
  }
};

}  // namespace hop

#endif  // JASMIN_INSTRUCTIONS_BOOL_H
