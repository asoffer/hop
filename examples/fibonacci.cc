#include <cinttypes>
#include <cstdio>
#include <cstring>

#include "jasmin/core/program.h"
#include "jasmin/instructions/arithmetic.h"
#include "jasmin/instructions/common.h"
#include "jasmin/instructions/compare.h"
#include "nth/container/interval.h"
#include "nth/container/stack.h"

// This file defines two implementations of functions that compute fibonacci
// numbers recursively. The implementations have the same high-level pseudocode,
// but differ only in the instruction set they use. The first implementation
// uses subtraction to compute `n - 1` and `n - 2`, which it passes to the
// function recursively. The second implementation uses dynamic programming and
// a user-defined `UpdateFibonacci` function. The idea here is to show not
// only what directly working with Jasmin's byte code looks like, but also to
// point out that Jasmin allows users to fully control the instruction set they
// are working with.

using RecursiveInstructions = jasmin::MakeInstructionSet<
    jasmin::Duplicate, jasmin::Swap, jasmin::Push<uint64_t>,
    jasmin::Push<jasmin::Function<>*>, jasmin::LessThan<uint64_t>,
    jasmin::Add<uint64_t>, jasmin::Subtract<uint64_t>>;

jasmin::Program<RecursiveInstructions> FibonacciRecursive() {
  jasmin::Program<RecursiveInstructions> p;
  auto& func = p.declare("fib", 1, 1);
  func.append<jasmin::Duplicate>();
  func.append<jasmin::Push<uint64_t>>(2);
  func.append<jasmin::LessThan<uint64_t>>();
  nth::interval<jasmin::InstructionIndex> jump =
      func.append_with_placeholders<jasmin::JumpIf>();
  func.append<jasmin::Duplicate>();
  func.append<jasmin::Push<uint64_t>>(1);
  func.append<jasmin::Subtract<uint64_t>>();
  func.append<jasmin::Push<jasmin::Function<>*>>(&func);
  func.append<jasmin::Call>(
      jasmin::InstructionSpecification{.parameters = 1, .returns = 1});
  func.append<jasmin::Swap>();
  func.append<jasmin::Push<uint64_t>>(2);
  func.append<jasmin::Subtract<uint64_t>>();
  func.append<jasmin::Push<jasmin::Function<>*>>(&func);
  func.append<jasmin::Call>(
      jasmin::InstructionSpecification{.parameters = 1, .returns = 1});
  func.append<jasmin::Add<uint64_t>>();
  nth::interval<jasmin::InstructionIndex> ret = func.append<jasmin::Return>();
  func.set_value(jump, 0, ret.lower_bound() - jump.lower_bound());
  return p;
}

struct UpdateFibonacci : jasmin::Instruction<UpdateFibonacci> {
  static std::array<jasmin::Value, 3> consume(
      std::span<jasmin::Value, 3> values) {
    auto n = values[0];
    auto a = values[1];
    auto b = values[2];
    return {static_cast<uint64_t>(n.as<uint64_t>() - 1), b,
            a.as<uint64_t>() + b.as<uint64_t>()};
  }
};

using DynamicInstructions =
    jasmin::MakeInstructionSet<jasmin::DuplicateAt, jasmin::Push<uint64_t>,
                               jasmin::Equal<uint64_t>, UpdateFibonacci>;

jasmin::Program<DynamicInstructions> FibonacciDynamicProgramming() {
  jasmin::Program<DynamicInstructions> p;
  auto& func = p.declare("fib", 1, 1);
  func.append<jasmin::Push<uint64_t>>(1);
  func.append<jasmin::Push<uint64_t>>(0);
  auto loop_start = func.append<jasmin::DuplicateAt>(
      jasmin::InstructionSpecification{.parameters = 3, .returns = 1});
  func.append<jasmin::Push<uint64_t>>(0);
  func.append<jasmin::Equal<uint64_t>>();
  nth::interval<jasmin::InstructionIndex> jump =
      func.append_with_placeholders<jasmin::JumpIf>();
  func.append<UpdateFibonacci>();
  nth::interval<jasmin::InstructionIndex> loop_end =
      func.append_with_placeholders<jasmin::Jump>();
  nth::interval<jasmin::InstructionIndex> ret = func.append<jasmin::Return>();

  func.set_value(loop_end, 0,
                 loop_start.lower_bound() - loop_end.lower_bound());
  func.set_value(jump, 0, ret.lower_bound() - jump.lower_bound());
  return p;
}

int main(int argc, char const* argv[]) {
  if (argc != 3) {
    std::fputs(R"(Usage:

    fibonacci <implementation> <number>

      <implementation>  -- One of "recursive" or "dynamic". See source file for
                           details.
      <number>          -- For a number `n`, computes the `n`th fibonacci number
)",
               stderr);
    return 1;
  }

  nth::stack<jasmin::Value> stack = {std::atoi(argv[2])};
  if (std::strcmp(argv[1], "recursive") == 0) {
    auto program = FibonacciRecursive();
    program.function("fib").invoke(stack);
  } else if (std::strcmp(argv[1], "dynamic") == 0) {
    auto program = FibonacciDynamicProgramming();
    program.function("fib").invoke(stack);
  } else {
    std::fputs("Choose an implementation: \"recursive\" or \"dynamic\".",
               stderr);
    return 1;
  }

  uint64_t result = stack.top().as<uint64_t>();
  std::printf("[[%" PRIu64 "]]\n", result);
  return 0;
}
