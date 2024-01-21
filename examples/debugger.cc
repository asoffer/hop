#include <cstdio>

#include "jasmin/core/debugger.h"
#include "jasmin/core/function.h"
#include "jasmin/core/program.h"
#include "jasmin/instructions/common.h"
#include "nth/container/stack.h"

// This file a program consisting of some trivial functions which just call one
// another. It is designed to show the most basic usage of Jasmin's debugger
// functionality.

using Instructions = jasmin::MakeInstructionSet<jasmin::Push<jasmin::Function<>*>>;
using Program = jasmin::Program<Instructions>;

// Constructns a program with four functions named `a`, `b`, `c`, and `d`. The
// functions call each other, with `a` calling `b` twice, `b` calling `c` twice, and `c`
// calling `d`.
Program ConstructProgram() {
  Program p;
  auto& a = p.declare("a", 0, 0).function;
  auto& b = p.declare("b", 0, 0).function;
  auto& c = p.declare("c", 0, 0).function;
  auto& d = p.declare("d", 0, 0).function;

  // `a` calls `b` twice and then returns.
  a.append<jasmin::Push<jasmin::Function<>*>>(&b);
  a.append<jasmin::Call>({});
  a.append<jasmin::Push<jasmin::Function<>*>>(&b);
  a.append<jasmin::Call>({});
  a.append<jasmin::Return>();

  // `b` calls `c` twice and then returns.
  b.append<jasmin::Push<jasmin::Function<>*>>(&c);
  b.append<jasmin::Call>({});
  b.append<jasmin::Push<jasmin::Function<>*>>(&c);
  b.append<jasmin::Call>({});
  b.append<jasmin::Return>();

  // `c` calls `d` once and then returns.
  c.append<jasmin::Push<jasmin::Function<>*>>(&d);
  c.append<jasmin::Call>({});
  c.append<jasmin::Return>();

  // `d` does nothing, returning immediately.
  d.append<jasmin::Return>();
  
  return p;
}

int main() {
  // Create a program.
  Program program = ConstructProgram();

  // Construct a debugger and attach it to `program`.
  jasmin::Debugger debugger(program);

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
  nth::stack<jasmin::Value> stack;
  program.function("a").invoke(stack);

  return 0;
}

