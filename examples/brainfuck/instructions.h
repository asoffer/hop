#ifndef EXAMPLES_BRAINFUCK_INSTRUCTIONS_H
#define EXAMPLES_BRAINFUCK_INSTRUCTIONS_H

#include "hop/core/instruction.h"
#include "nth/debug/debug.h"

namespace bf {

struct State {
  static constexpr size_t buffer_size = 30'000;
  uint8_t buffer[buffer_size]         = {0};
};

struct Initialize : hop::Instruction<Initialize> {
  using function_state = State;
  static void execute(function_state &state, hop::Input<>,
                      hop::Output<uint8_t *> out) {
    out.set(state.buffer);
  }
};

struct Increment : hop::Instruction<Increment> {
  using function_state = State;
  static void execute(function_state &, hop::Input<uint8_t *> in,
                      hop::Output<>) {
    ++*in.get<0>();
  }
};

struct Decrement : hop::Instruction<Decrement> {
  using function_state = State;
  static void execute(function_state &, hop::Input<uint8_t *> in,
                      hop::Output<>) {
    --*in.get<0>();
  }
};

struct Left : hop::Instruction<Left> {
  using function_state = State;
  static void consume(function_state &state, hop::Input<uint8_t *> in,
                      hop::Output<uint8_t *> out) {
    auto [ptr] = in;
    NTH_REQUIRE(ptr != state.buffer);
    out.set(ptr - 1);
  }
};

struct Right : hop::Instruction<Right> {
  using function_state = State;
  static void consume(function_state &state, hop::Input<uint8_t *> in,
                      hop::Output<uint8_t *> out) {
    auto [ptr] = in;
    NTH_REQUIRE(ptr < state.buffer + state.buffer_size);
    out.set(ptr + 1);
  }
};

struct Input : hop::Instruction<Input> {
  using function_state = State;
  static void execute(function_state &, hop::Input<uint8_t *> in,
                      hop::Output<>) {
    auto [ptr] = in;
    *ptr       = static_cast<uint8_t>(std::getchar());
  }
};

struct Output : hop::Instruction<Output> {
  using function_state = State;
  static void execute(function_state &, hop::Input<uint8_t *> in,
                      hop::Output<>) {
    std::putchar(static_cast<char>(*in.get<0>()));
  }
};

struct Zero : hop::Instruction<Zero> {
  using function_state = State;
  static void execute(function_state &, hop::Input<uint8_t *> in,
                      hop::Output<bool> out) {
    out.set(*in.get<0>() == 0);
  }
};

using Instructions =
    hop::MakeInstructionSet<Initialize, Increment, Decrement, Left, Right,
                               Zero, Input, Output>;

}  // namespace bf

#endif  // EXAMPLES_BRAINFUCK_INSTRUCTIONS_H
