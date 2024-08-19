#include "hop/core/debugger.h"

#include <cstdio>

#include "hop/core/function.h"
#include "hop/core/program_fragment.h"
#include "hop/instructions/common.h"
#include "nth/container/stack.h"

// This file a program consisting of some trivial functions which just call one
// another. It is designed to show the most basic usage of Hop's debugger
// functionality.

using Instructions =
    hop::MakeInstructionSet<hop::Push<hop::Function<>*>>;
using ProgramFragment = hop::ProgramFragment<Instructions>;

// Constructns a program with four functions named `a`, `b`, `c`, and `d`. The
// functions call each other, with `a` calling `b` twice, `b` calling `c` twice,
// and `c` calling `d`.
ProgramFragment ConstructProgram() {
  ProgramFragment p;
  auto& a = p.declare("a", 0, 0).function;
  auto& b = p.declare("b", 0, 0).function;
  auto& c = p.declare("c", 0, 0).function;
  auto& d = p.declare("d", 0, 0).function;

  // `a` calls `b` twice and then returns.
  a.append<hop::Push<hop::Function<>*>>(&b);
  a.append<hop::Call>({});
  a.append<hop::Push<hop::Function<>*>>(&b);
  a.append<hop::Call>({});
  a.append<hop::Return>();

  // `b` calls `c` twice and then returns.
  b.append<hop::Push<hop::Function<>*>>(&c);
  b.append<hop::Call>({});
  b.append<hop::Push<hop::Function<>*>>(&c);
  b.append<hop::Call>({});
  b.append<hop::Return>();

  // `c` calls `d` once and then returns.
  c.append<hop::Push<hop::Function<>*>>(&d);
  c.append<hop::Call>({});
  c.append<hop::Return>();

  // `d` does nothing, returning immediately.
  d.append<hop::Return>();

  return p;
}

int main() {
  // Create a program.
  ProgramFragment program = ConstructProgram();

  // Construct a debugger and attach it to `program`.
  hop::Debugger debugger(program);

  int counter = 0;

  // Set a breakpoint at the function named `c`. Our lambda will be invoked each
  // time our breakpoint is hit, and continue executing after our lambda
  // returns.
  debugger.set_function_breakpoint("c", [&] {
    std::printf(
        "Debugger hit a breakpoint %d times!\n"
        "Press enter to continue...\n",
        ++counter);
    (void)std::getchar();
  });

  // Start executing the function `a`. This function will call `b` which will in
  // turn call `c`, hitting the breakpoint. Afterwards, it will continue to `d`,
  // return back through `c`, arriving at a second call to `c`, which will once
  // again trigger the breakpoint. This will happen two more times for the
  // second call to `b` in `a`, resulting in a total of four breakpoint
  // triggerings.
  nth::stack<hop::Value> stack;
  program.function("a").invoke(stack);

  return 0;
}

