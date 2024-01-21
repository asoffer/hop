#ifndef JASMIN_INSTRUCTIONS_COMMON_H
#define JASMIN_INSTRUCTIONS_COMMON_H

#include "jasmin/core/instruction.h"
#include "jasmin/ssa/register_coalescer.h"
#include "jasmin/ssa/ssa.h"

namespace jasmin {

template <typename T>
struct Push : Instruction<Push<T>> {
  static std::string_view name() { return "push"; }

  static constexpr T execute(std::span<Value, 0>, T v) { return v; }

  static std::string debug(std::span<Value const, 1> immediates) {
    return "push " + std::to_string(immediates[0].raw_value());
  }

  static void identify(RegisterCoalescer& p, SsaInstruction const& i) {
    p.identify(i.output(0), i.argument(0));
  }
};

struct Drop : Instruction<Drop> {
  static std::string_view name() { return "drop"; }
  static constexpr void consume(std::span<Value, 1>) {}
  static std::string debug(std::span<Value const, 0>) { return "drop"; }
};

struct Swap : Instruction<Swap> {
  static void execute(std::span<Value, 2> values) {
    std::swap(values[0], values[1]);
  }

  static constexpr std::string_view debug() { return "swap"; }

  static void identify(RegisterCoalescer& p, SsaInstruction const& i) {
    p.identify(i.output(0), i.argument(1));
    p.identify(i.output(1), i.argument(0));
  }
};

struct Rotate : Instruction<Rotate> {
  static std::string_view name() { return "rotate"; }

  static constexpr void execute(std::span<Value> in,
                                [[maybe_unused]] std::span<Value> out,
                                size_t amount) {
    std::rotate(in.begin(), in.begin() + amount, in.end());
  }

  static std::string debug(std::span<Value const, 1> immediates) {
    return "rotate " + std::to_string(immediates[0].as<size_t>());
  }
};

struct Duplicate : Instruction<Duplicate> {
  static std::string_view name() { return "duplicate"; }
  static Value execute(std::span<Value, 1> values) { return values[0]; }
  static constexpr std::string_view debug() { return "duplicate"; }

  static void identify(RegisterCoalescer& p, SsaInstruction const& i) {
    p.identify(i.output(0), i.argument(0));
    p.identify(i.output(1), i.argument(0));
  }
};

struct DuplicateAt : Instruction<DuplicateAt> {
  static std::string_view name() { return "duplicate-at"; }
  static void execute(std::span<Value> in, std::span<Value> out) {
    out.front() = in.front();
  }
  static std::string debug(std::span<Value const, 1> immediates) {
    return "duplicate @" + std::to_string(immediates[0].as<size_t>());
  }
};

struct Load : Instruction<Load> {
  static std::string_view debug() { return "load"; }
  static Value consume(std::span<Value, 1> values, size_t size) {
    return Value::Load(values[0].as<std::byte const*>(), size);
  }
};

struct Store : Instruction<Store> {
  static void consume(std::span<Value, 2> values, uint8_t size) {
    Value::Store(values[1], values[0].as<void*>(), size);
  }
  static constexpr std::string_view debug() { return "store"; }
};

}  // namespace jasmin

#endif  // JASMIN_INSTRUCTIONS_COMMON_H
