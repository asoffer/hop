#include <cstdio>
#include <iostream>
#include <queue>

#include "jasmin/execute.h"
#include "jasmin/instructions/core.h"

// This file provides an example of a Jasmin function that hold mutable state
// associated with each invoked function. This example is slightly more advanced
// and we suggest looking at the "hello_world.cc" and "fibonacci.cc" examples
// before this one.

struct PrintCString : jasmin::StackMachineInstruction<PrintCString> {
  static void execute(char const *cstr) { std::fputs(cstr, stdout); }
};

// Below we define the `PushQueue` and `PopQueue` instructions. These two
// instructions specify state via the `JasminFunctionState` type alias. Any
// instructions that name the same state type will share access to the same
// state, so it is considered best practice to choose specific state types so as
// not to create accidental conflicts.
//
// For this reason, even though
// `using JasminFunctionState = std::queue<char const *>` would suffice for our
// needs with `PushQueue` and `PopQueue`, we choose to create a wrapper type.
struct Queue {
  std::queue<char const *> queue;
};

struct PushQueue : jasmin::StackMachineInstruction<PushQueue> {
  using JasminFunctionState = Queue;
  static void execute(jasmin::ValueStack &, JasminFunctionState &state,
                      char const *cstr) {
    state.queue.push(cstr);
  }
};

struct PopQueue : jasmin::StackMachineInstruction<PopQueue> {
  using JasminFunctionState = Queue;
  static char const *execute(JasminFunctionState &state) {
    char const *result = state.queue.front();
    state.queue.pop();
    return result;
  }
};

struct RotateQueue : jasmin::StackMachineInstruction<RotateQueue> {
  using JasminFunctionState = Queue;
  static void execute(JasminFunctionState &state) {
    char const *top = state.queue.front();
    state.queue.pop();
    state.queue.push(top);
  }
};

void HelloWorld() {
  using Instructions = jasmin::MakeInstructionSet<PrintCString, PushQueue,
                                                  PopQueue, RotateQueue>;

  jasmin::Function<Instructions> func(0, 0);

  // Push "world!\n" onto the state queue.
  func.append<PushQueue>("world!\n");

  // Push "Hello, !" onto the state queue. The queue now holds these elements in
  // front-to-back order: {"world!\n", "Hello, "}
  func.append<PushQueue>("Hello, ");

  // Rotates the first element to the back of the queue. The queue now holds
  // these elements in front-to-back order: {"Hello, ", "world!\n"}
  func.append<RotateQueue>();

  // Pops "Hello, " from the state queue and onto the value-stack.
  func.append<PopQueue>();

  func.append<PrintCString>();

  // Pops "world!\n" from the state queue and onto the value-stack.
  func.append<PopQueue>();

  func.append<PrintCString>();

  func.append<jasmin::Return>();

  // Now that our function has been defined, we can execute it.
  jasmin::Execute(func, {/* No arguments */});
}

int main() {
  std::fputs(
      "Executing HelloWorld...\n"
      "=======================\n",
      stdout);
  HelloWorld();
  std::fputs(
      "=======================\n"
      "...done!\n",
      stdout);

  return 0;
}