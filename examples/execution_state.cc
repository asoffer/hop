#include <cstdio>
#include <iostream>
#include <queue>

#include "jasmin/execute.h"
#include "jasmin/instructions/core.h"

// This file provides an example of a Jasmin function that hold mutable state
// associated the entire execution stack, persisted across multiple function
// calls. This example is slightly more advanced and we suggest looking at the
// "hello_world.cc" and "fibonacci.cc" examples before this one.

struct IncrementCount : jasmin::StackMachineInstruction<IncrementCount> {
  using JasminExecutionState = int;
  static void execute(JasminExecutionState &state) { state++; }
};

struct PushCount : jasmin::StackMachineInstruction<PushCount> {
  using JasminExecutionState = int;
  static int execute(JasminExecutionState &state) { return state; }
};

struct PrintInt : jasmin::StackMachineInstruction<PrintInt> {
  static void execute(int n) { std::printf("%d\n", n); }
};

// Sets up three functions `f1`, `f2`, and `f3`. The first, `f1`' increments the
// counter, prints it, calls `f2`, and prints the counter again. Similarly `f2`
// increments the counter, prints it, calls `f3`, and prints the counter again.
// The function `f3` just increments the counter and prints it. We expect the
// values 1, 2, 3, 3, 3 to be printed in order because the counter's value is
// retained across function invocations. Executing it a second time with the
// same state, we expect the values to be 4, 5, 6, 6, 6.
void Counter() {
  using Instructions = jasmin::MakeInstructionSet<IncrementCount, PushCount,
                                                  PrintInt, jasmin::Push>;

  jasmin::Function<Instructions> f1(0, 0);
  jasmin::Function<Instructions> f2(0, 0);
  jasmin::Function<Instructions> f3(0, 0);

  f1.append<IncrementCount>();
  f1.append<PushCount>();
  f1.append<PrintInt>();
  f1.append<jasmin::Push>(&f2);
  f1.append<jasmin::Call>();
  f1.append<PushCount>();
  f1.append<PrintInt>();
  f1.append<jasmin::Return>();

  f2.append<IncrementCount>();
  f2.append<PushCount>();
  f2.append<PrintInt>();
  f2.append<jasmin::Push>(&f3);
  f2.append<jasmin::Call>();
  f2.append<PushCount>();
  f2.append<PrintInt>();
  f2.append<jasmin::Return>();

  f3.append<IncrementCount>();
  f3.append<PushCount>();
  f3.append<PrintInt>();
  f3.append<jasmin::Return>();

  int n;
  jasmin::ExecutionState<Instructions> state(n);

  // Now that our function has been defined, we can execute it.
  jasmin::Execute(f1, state, {/* No arguments */});
  jasmin::Execute(f1, state, {/* No arguments */});
}

int main() {
  std::fputs(
      "Executing Counter ...\n"
      "=======================\n",
      stdout);
  Counter();
  std::fputs(
      "=======================\n"
      "...done!\n",
      stdout);

  return 0;
}
