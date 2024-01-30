#ifndef EXAMPLES_BRAINFUCK_INSTRUCTIONS_H
#define EXAMPLES_BRAINFUCK_INSTRUCTIONS_H

#include "jasmin/compile/x64/code_generator.h"
#include "jasmin/compile/x64/location_map.h"
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

  static void generate_code(jasmin::x64::CodeGenerator &gen,
                            jasmin::LocationMap const &) {
    gen.write({0x48, 0x81, 0xec, 0x30, 0x75, 0x00, 0x00});  // sub rsp, 0x7530
    gen.mov(jasmin::x64::Register::rsi, jasmin::x64::Register::rsp);
    gen.write({
        0xba, 0x01, 0x00, 0x00, 0x00,              // mov edx, 0x1
        0x48, 0x89, 0xe7,                          // mov rdi, rsp
        0x31, 0xc0,                                // xor eax eax
        0x48, 0xc7, 0xc1, 0x30, 0x75, 0x00, 0x00,  // mov rcx, 0x1000
        0xf3, 0xaa,  // rep stos BYTE PTR es:[rdi],al
    });
  }
};

struct Increment : jasmin::Instruction<Increment> {
  using function_state = State;
  static void execute(function_state &, jasmin::Input<uint8_t *> in,
                      jasmin::Output<>) {
    ++*in.get<0>();
  }

  static void generate_code(jasmin::x64::CodeGenerator &gen,
                            jasmin::LocationMap const &) {
    gen.write({0x80, 0x06, 0x01});  // add BYTE PTR [rsi], 0x1
  }
};

struct Decrement : jasmin::Instruction<Decrement> {
  using function_state = State;
  static void execute(function_state &, jasmin::Input<uint8_t *> in,
                      jasmin::Output<>) {
    --*in.get<0>();
  }

  static void generate_code(jasmin::x64::CodeGenerator &gen,
                            jasmin::LocationMap const &) {
    gen.write({0x80, 0x2e, 0x01});  // sub BYTE PTR [rsi], 0x1
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

  static void generate_code(jasmin::x64::CodeGenerator &gen,
                            jasmin::LocationMap const &) {
    gen.write({0x48, 0x8d, 0x76, 0xff});  // lea rsi, [rsi - 1]
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

  static void generate_code(jasmin::x64::CodeGenerator &gen,
                            jasmin::LocationMap const &) {
    gen.write({0x48, 0x8d, 0x76, 0x01});  // lea rsi, [rsi + 1]
  }
};

struct Input : jasmin::Instruction<Input> {
  using function_state = State;
  static void execute(function_state &, jasmin::Input<uint8_t *> in,
                      jasmin::Output<>) {
    auto [ptr] = in;
    *ptr       = static_cast<uint8_t>(std::getchar());
  }

  static void generate_code(jasmin::x64::CodeGenerator &gen,
                            jasmin::LocationMap const &) {
    gen.write({
        0x48, 0xc7, 0xc0, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0
        0x48, 0xc7, 0xc7, 0x00, 0x00, 0x00, 0x00,  // mov rdi, 0x0
        0x48, 0xc7, 0xc2, 0x01, 0x00, 0x00, 0x00,  // mov rdx, 0x1
    });
    gen.syscall();
  }
};

struct Output : jasmin::Instruction<Output> {
  using function_state = State;
  static void execute(function_state &, jasmin::Input<uint8_t *> in,
                      jasmin::Output<>) {
    std::putchar(static_cast<char>(*in.get<0>()));
  }

  static void generate_code(jasmin::x64::CodeGenerator &gen,
                            jasmin::LocationMap const &) {
    gen.write({
        0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00,  // mov rax, 0x1
        0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00,  // mov rdi, 0x1
        0x48, 0xc7, 0xc2, 0x01, 0x00, 0x00, 0x00,  // mov rdx, 0x1
    });
    gen.syscall();
  }
};

struct Zero : jasmin::Instruction<Zero> {
  using function_state = State;
  static void execute(function_state &, jasmin::Input<uint8_t *> in,
                      jasmin::Output<bool> out) {
    out.set(*in.get<0>() == 0);
  }

  static void generate_code(jasmin::x64::CodeGenerator &gen,
                            jasmin::LocationMap const &) {
    gen.write({0x8a, 0x06});  // mov al, BYTE PTR [rsi]
  }
};

using Instructions =
    jasmin::MakeInstructionSet<Initialize, Increment, Decrement, Left, Right,
                               Zero, Input, Output>;

}  // namespace bf

#endif  // EXAMPLES_BRAINFUCK_INSTRUCTIONS_H
