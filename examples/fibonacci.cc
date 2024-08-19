#include <cinttypes>
#include <cstdio>
#include <cstring>

#include "hop/core/program_fragment.h"
#include "hop/instructions/arithmetic.h"
#include "hop/instructions/common.h"
#include "hop/instructions/compare.h"
#include "nth/container/interval.h"
#include "nth/container/stack.h"

// This file defines two implementations of functions that compute fibonacci
// numbers recursively. The implementations have the same high-level pseudocode,
// but differ only in the instruction set they use. The first implementation
// uses subtraction to compute `n - 1` and `n - 2`, which it passes to the
// function recursively. The second implementation uses dynamic programming and
// a user-defined `UpdateFibonacci` function. The idea here is to show not
// only what directly working with Hop's byte code looks like, but also to
// point out that Hop allows users to fully control the instruction set they
// are working with.

using RecursiveInstructions = hop::MakeInstructionSet<
    hop::Duplicate, hop::Swap, hop::Push<uint64_t>,
    hop::Push<hop::Function<>*>, hop::LessThan<uint64_t>,
    hop::Add<uint64_t>, hop::Subtract<uint64_t>>;

hop::ProgramFragment<RecursiveInstructions> FibonacciRecursive() {
  hop::ProgramFragment<RecursiveInstructions> p;
  auto& func = p.declare("fib", 1, 1).function;
  func.append<hop::Duplicate>();
  func.append<hop::Push<uint64_t>>(2);
  func.append<hop::LessThan<uint64_t>>();
  nth::interval<hop::InstructionIndex> jump =
      func.append_with_placeholders<hop::JumpIf>();
  func.append<hop::Duplicate>();
  func.append<hop::Push<uint64_t>>(1);
  func.append<hop::Subtract<uint64_t>>();
  func.append<hop::Push<hop::Function<>*>>(&func);
  func.append<hop::Call>(
      hop::InstructionSpecification{.parameters = 1, .returns = 1});
  func.append<hop::Swap>();
  func.append<hop::Push<uint64_t>>(2);
  func.append<hop::Subtract<uint64_t>>();
  func.append<hop::Push<hop::Function<>*>>(&func);
  func.append<hop::Call>(
      hop::InstructionSpecification{.parameters = 1, .returns = 1});
  func.append<hop::Add<uint64_t>>();
  nth::interval<hop::InstructionIndex> ret = func.append<hop::Return>();
  func.set_value(jump, 0, ret.lower_bound() - jump.lower_bound());
  return p;
}

struct UpdateFibonacci : hop::Instruction<UpdateFibonacci> {
  static void consume(hop::Input<uint64_t, uint64_t, uint64_t> in,
                      hop::Output<uint64_t, uint64_t, uint64_t> out) {
    out.set<0>(in.get<0>() - 1);
    out.set<1>(in.get<2>());
    out.set<2>(in.get<1>() + in.get<2>());
  }
};

using DynamicInstructions =
    hop::MakeInstructionSet<hop::DuplicateAt, hop::Push<uint64_t>,
                               hop::Equal<uint64_t>, UpdateFibonacci>;

hop::ProgramFragment<DynamicInstructions> FibonacciDynamicProgramming() {
  hop::ProgramFragment<DynamicInstructions> p;
  auto [id, func] = p.declare("fib", 1, 1);
  func.append<hop::Push<uint64_t>>(1);
  func.append<hop::Push<uint64_t>>(0);
  auto loop_start = func.append<hop::DuplicateAt>(
      hop::InstructionSpecification{.parameters = 3, .returns = 1});
  func.append<hop::Push<uint64_t>>(0);
  func.append<hop::Equal<uint64_t>>();
  nth::interval<hop::InstructionIndex> jump =
      func.append_with_placeholders<hop::JumpIf>();
  func.append<UpdateFibonacci>();
  nth::interval<hop::InstructionIndex> loop_end =
      func.append_with_placeholders<hop::Jump>();
  nth::interval<hop::InstructionIndex> ret = func.append<hop::Return>();

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

  nth::stack<hop::Value> stack = {std::atoi(argv[2])};
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
