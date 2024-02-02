#ifndef EXAMPLES_BRAINFUCK_INSTRUCTIONS_H
#define EXAMPLES_BRAINFUCK_INSTRUCTIONS_H

#include "jasmin/core/instruction.h"
#include "nth/debug/debug.h"

namespace bf {

struct State {
  static constexpr size_t buffer_size = 30'000;
  uint8_t buffer[buffer_size]         = {0};
};

struct Initialize : jasmin::Instruction<Initialize> {
  using function_state = State;
  static void execute(function_state &state, jasmin::Input<>,
                      jasmin::Output<uint8_t *> out) {
    out.set(state.buffer);
  }
};

struct Increment : jasmin::Instruction<Increment> {
  using function_state = State;
  static void execute(function_state &, jasmin::Input<uint8_t *> in,
                      jasmin::Output<>) {
    ++*in.get<0>();
  }
};

struct Decrement : jasmin::Instruction<Decrement> {
  using function_state = State;
  static void execute(function_state &, jasmin::Input<uint8_t *> in,
                      jasmin::Output<>) {
    --*in.get<0>();
  }
};

struct Left : jasmin::Instruction<Left> {
  using function_state = State;
  static void consume(function_state &state, jasmin::Input<uint8_t *> in,
                      jasmin::Output<uint8_t *> out) {
    auto [ptr] = in;
    NTH_REQUIRE(ptr != state.buffer);
    out.set(ptr - 1);
  }
};

struct Right : jasmin::Instruction<Right> {
  using function_state = State;
  static void consume(function_state &state, jasmin::Input<uint8_t *> in,
                      jasmin::Output<uint8_t *> out) {
    auto [ptr] = in;
    NTH_REQUIRE(ptr < state.buffer + state.buffer_size);
    out.set(ptr + 1);
  }
};

struct Input : jasmin::Instruction<Input> {
  using function_state = State;
  static void execute(function_state &, jasmin::Input<uint8_t *> in,
                      jasmin::Output<>) {
    auto [ptr] = in;
    *ptr       = static_cast<uint8_t>(std::getchar());
  }
};

struct Output : jasmin::Instruction<Output> {
  using function_state = State;
  static void execute(function_state &, jasmin::Input<uint8_t *> in,
                      jasmin::Output<>) {
    std::putchar(static_cast<char>(*in.get<0>()));
  }
};

struct Zero : jasmin::Instruction<Zero> {
  using function_state = State;
  static void execute(function_state &, jasmin::Input<uint8_t *> in,
                      jasmin::Output<bool> out) {
    out.set(*in.get<0>() == 0);
  }
};

using Instructions =
    jasmin::MakeInstructionSet<Initialize, Increment, Decrement, Left, Right,
                               Zero, Input, Output>;

}  // namespace bf

#endif  // EXAMPLES_BRAINFUCK_INSTRUCTIONS_H
