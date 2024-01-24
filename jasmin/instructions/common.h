#ifndef JASMIN_INSTRUCTIONS_COMMON_H
#define JASMIN_INSTRUCTIONS_COMMON_H

#include "jasmin/core/instruction.h"
#include "jasmin/ssa/register_coalescer.h"
#include "jasmin/ssa/ssa.h"

namespace jasmin {

template <typename T>
struct Push : Instruction<Push<T>> {
  static constexpr void execute(std::span<Value, 0>, std::span<Value, 1> out,
                                T v) {
    out[0] = v;
  }

  static void identify(RegisterCoalescer& p, SsaInstruction const& i) {
    p.identify(i.output(0), i.argument(0));
  }
};

struct Drop : Instruction<Drop> {
  static constexpr void consume(std::span<Value, 1>, std::span<Value, 0>) {}
};

struct Swap : Instruction<Swap> {
  static void execute(std::span<Value, 2> values, std::span<Value, 0>) {
    std::swap(values[0], values[1]);
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
  static void execute(std::span<Value, 1> values, std::span<Value, 1> out) {
    out[0] = values[0];
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
  static void execute(std::span<Value, 1> values, std::span<Value, 0>,
                      size_t size) {
    values[0] = Value::Load(values[0].as<std::byte const*>(), size);
  }
};

struct Store : Instruction<Store> {
  static void consume(std::span<Value, 2> values, std::span<Value, 0>,
                      uint8_t size) {
    Value::Store(values[1], values[0].as<void*>(), size);
  }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_COMMON_H
