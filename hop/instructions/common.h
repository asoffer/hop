#ifndef JASMIN_INSTRUCTIONS_COMMON_H
#define JASMIN_INSTRUCTIONS_COMMON_H

#include "hop/core/instruction.h"
#include "hop/ssa/register_coalescer.h"
#include "hop/ssa/ssa.h"

namespace hop {

template <typename T>
struct Push : Instruction<Push<T>> {
  static constexpr void execute(Input<>, Output<T> out, T v) {
    out.template set<0>(v);
  }

  static void identify(RegisterCoalescer& p, SsaInstruction const& i) {
    p.identify(i.output(0), i.argument(0));
  }
};

struct Drop : Instruction<Drop> {
  static constexpr void consume(Input<Value>, Output<>) {}
};

struct Swap : Instruction<Swap> {
  static void consume(Input<Value, Value> in, Output<Value, Value>out) {
    out.set<0>(in.get<1>());
    out.set<1>(in.get<0>());
  }

  static void identify(RegisterCoalescer& p, SsaInstruction const& i) {
    p.identify(i.output(0), i.argument(1));
    p.identify(i.output(1), i.argument(0));
  }
};

struct Rotate : Instruction<Rotate> {
  static constexpr void execute(std::span<Value> in,
                                [[maybe_unused]] std::span<Value> out,
                                size_t amount) {
    std::rotate(in.begin(), in.begin() + amount, in.end());
  }
};

struct Duplicate : Instruction<Duplicate> {
  static void execute(Input<Value> in, Output<Value> out) {
    out.set<0>(in.get<0>());
  }

  static void identify(RegisterCoalescer& p, SsaInstruction const& i) {
    p.identify(i.output(0), i.argument(0));
    p.identify(i.output(1), i.argument(0));
  }
};

struct DuplicateAt : Instruction<DuplicateAt> {
  static void execute(std::span<Value> in, std::span<Value> out) {
    out.front() = in.front();
  }
};

struct Load : Instruction<Load> {
  static void consume(Input<std::byte const*> in, Output<Value> out,
                      size_t size) {
    out.set<0>(Value::Load(in.get<0>(), size));
  }
};

struct Store : Instruction<Store> {
  static void consume(Input<void*, Value> in, Output<>, uint8_t size) {
    Value::Store(in.get<1>(), in.get<0>(), size);
  }
};

}  // namespace hop

#endif  // JASMIN_INSTRUCTIONS_COMMON_H
