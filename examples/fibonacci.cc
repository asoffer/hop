#include <cinttypes>
#include <cstdio>
#include <cstring>

#include "jasmin/execute.h"
#include "jasmin/instructions/arithmetic.h"
#include "jasmin/instructions/compare.h"
#include "jasmin/instructions/core.h"

// This file defines two implementations of functions that compute fibonacci
// numbers recursively. The implementations have the same high-level pseudocode,
// but differ only in the instruction set they use. The first implementation
// uses subtraction to compute `n - 1` and `n - 2`, which it passes to the
// function recursively. The second implementation has built-in instructions for
// "decrement-by-one" and "decrement-by-two". The idea here is to show, not only
// what directly working with Jasmin's byte code looks like, but also to point
// out that Jasmin allows users to fully control the instruction set they are
// working with.

auto FibonacciWithSubtract() {
  using Instructions = jasmin::MakeInstructionSet<
      jasmin::Return, jasmin::Duplicate, jasmin::Swap, jasmin::JumpIf,
      jasmin::Call, jasmin::Push, jasmin::LessThan<uint64_t>,
      jasmin::Add<uint64_t>, jasmin::Subtract<uint64_t>,
      jasmin::Multiply<uint64_t>>;
  jasmin::Function<Instructions> func(1, 1);
  func.append<jasmin::Duplicate>();
  func.append<jasmin::Push>(uint64_t{2});
  func.append<jasmin::LessThan<uint64_t>>();
  auto index = func.append_with_placeholders<jasmin::JumpIf>(1);
  func.append<jasmin::Duplicate>();
  func.append<jasmin::Push>(uint64_t{1});
  func.append<jasmin::Subtract<uint64_t>>();
  func.append<jasmin::Push>(&func);
  func.append<jasmin::Call>();
  func.append<jasmin::Swap>();
  func.append<jasmin::Push>(uint64_t{2});
  func.append<jasmin::Subtract<uint64_t>>();
  func.append<jasmin::Push>(&func);
  func.append<jasmin::Call>();
  func.append<jasmin::Add<uint64_t>>();
  func.append<jasmin::Return>();
  func.set_value(index, static_cast<ptrdiff_t>(func.size() - index));
  return func;
}

template <typename T, T N>
struct DecrementBy : jasmin::StackMachineInstruction<DecrementBy<T, N>> {
  static constexpr T execute(T x) { return x - N; }
};

auto FibonacciWithHardCodedDecrements() {
  using Instructions = jasmin::MakeInstructionSet<
      jasmin::Return, jasmin::Duplicate, jasmin::Swap, jasmin::JumpIf,
      jasmin::Call, jasmin::Push, jasmin::LessThan<uint64_t>,
      jasmin::Add<uint64_t>, DecrementBy<uint64_t, 1>, DecrementBy<uint64_t, 2>,
      jasmin::Multiply<uint64_t>>;

  jasmin::Function<Instructions> func(1, 1);
  func.append<jasmin::Duplicate>();
  func.append<jasmin::Push>(uint64_t{2});
  func.append<jasmin::LessThan<uint64_t>>();
  auto index = func.append_with_placeholders<jasmin::JumpIf>(1);
  func.append<jasmin::Duplicate>();
  func.append<DecrementBy<uint64_t, 1>>();
  func.append<jasmin::Push>(&func);
  func.append<jasmin::Call>();
  func.append<jasmin::Swap>();
  func.append<DecrementBy<uint64_t, 2>>();
  func.append<jasmin::Push>(&func);
  func.append<jasmin::Call>();
  func.append<jasmin::Add<uint64_t>>();
  func.append<jasmin::Return>();
  func.set_value(index, static_cast<ptrdiff_t>(func.size() - index));
  return func;
}

int main(int argc, char const *argv[]) {
  if (argc != 3) {
    std::fputs(R"(Usage:

    fibonacci <implementation> <number>

      <implementation>  -- One of "subtract" or "decrement". See source file for
                           details.
      <number>          -- For a number `n`, computes the `n`th fibonacci number
)",
               stderr);
    return 1;
  }

  uint64_t input = std::atoi(argv[2]);
  uint64_t result;
  if (std::strcmp(argv[1], "subtract") == 0) {
    jasmin::Execute(FibonacciWithSubtract(), {input}, result);
  } else if (std::strcmp(argv[1], "decrement") == 0) {
    jasmin::Execute(FibonacciWithHardCodedDecrements(), {input}, result);
  } else {
    std::fputs("Choose an implementation: \"subtract\" or \"decrement\".",
               stderr);
    return 1;
  }

  std::printf("[[%" PRIu64 "]]\n", result);
  return 0;
}
