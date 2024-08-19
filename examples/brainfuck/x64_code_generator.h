#ifndef EXAMPLES_BRAINFUCK_X64_CODE_GENERATOR_H
#define EXAMPLES_BRAINFUCK_X64_CODE_GENERATOR_H

#include "hop/compile/x64/function_emitter.h"
#include "hop/compile/x64/location_map.h"
#include "nth/meta/type.h"

namespace bf {

struct X64CodeGenerator {
  void operator()(decltype(nth::type<Initialize>),
                  hop::x64::FunctionEmitter &gen,
                  hop::LocationMap const &) {
    gen.write({0x48, 0x81, 0xec, 0x30, 0x75, 0x00, 0x00});  // sub rsp, 0x7530
    gen.mov(hop::x64::Register::rsi, hop::x64::Register::rsp);
    gen.write({
        0xba, 0x01, 0x00, 0x00, 0x00,              // mov edx, 0x1
        0x48, 0x89, 0xe7,                          // mov rdi, rsp
        0x31, 0xc0,                                // xor eax eax
        0x48, 0xc7, 0xc1, 0x30, 0x75, 0x00, 0x00,  // mov rcx, 0x1000
        0xf3, 0xaa,  // rep stos BYTE PTR es:[rdi],al
    });
  }

  void operator()(decltype(nth::type<Increment>),
                  hop::x64::FunctionEmitter &gen,
                  hop::LocationMap const &) {
    gen.write({0x80, 0x06, 0x01});  // add BYTE PTR [rsi], 0x1
  }

  void operator()(decltype(nth::type<Decrement>),
                  hop::x64::FunctionEmitter &gen,
                  hop::LocationMap const &) {
    gen.write({0x80, 0x2e, 0x01});  // sub BYTE PTR [rsi], 0x1
  }

  void operator()(decltype(nth::type<Left>), hop::x64::FunctionEmitter &gen,
                  hop::LocationMap const &) {
    gen.write({0x48, 0x8d, 0x76, 0xff});  // lea rsi, [rsi - 1]
  }

  void operator()(decltype(nth::type<Right>), hop::x64::FunctionEmitter &gen,
                  hop::LocationMap const &) {
    gen.write({0x48, 0x8d, 0x76, 0x01});  // lea rsi, [rsi + 1]
  }

  void operator()(decltype(nth::type<Output>),
                  hop::x64::FunctionEmitter &gen,
                  hop::LocationMap const &) {
    gen.write({
        0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00,  // mov rax, 0x1
        0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00,  // mov rdi, 0x1
        0x48, 0xc7, 0xc2, 0x01, 0x00, 0x00, 0x00,  // mov rdx, 0x1
    });
    gen.syscall();
  }

  void operator()(decltype(nth::type<Input>), hop::x64::FunctionEmitter &gen,
                  hop::LocationMap const &) {
    gen.write({
        0x48, 0xc7, 0xc0, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0
        0x48, 0xc7, 0xc7, 0x00, 0x00, 0x00, 0x00,  // mov rdi, 0x0
        0x48, 0xc7, 0xc2, 0x01, 0x00, 0x00, 0x00,  // mov rdx, 0x1
    });
    gen.syscall();
  }

  void operator()(decltype(nth::type<Zero>), hop::x64::FunctionEmitter &gen,
                  hop::LocationMap const &) {
    gen.write({0x8a, 0x06});  // mov al, BYTE PTR [rsi]
  }
};

}  // namespace bf

#endif  // EXAMPLES_BRAINFUCK_X64_CODE_GENERATOR_H
