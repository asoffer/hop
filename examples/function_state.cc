#include <cstdio>
#include <queue>

#include "hop/instructions/common.h"
#include "nth/container/stack.h"

// This file provides an example of a Hop function that hold mutable state
// associated with each invoked function. This example is slightly more advanced
// and we suggest looking at the "hello_world.cc" and "fibonacci.cc" examples
// before this one.

struct PrintCString : hop::Instruction<PrintCString> {
  static void consume(hop::Input<char const *> in, hop::Output<>) {
    std::fputs(in.get<0>(), stdout);
  }
};

// Below we define the `PushQueue` and `PopQueue` instructions. These two
// instructions specify state via the `function_state` type alias. Any
// instructions that name the same state type will share access to the same
// state, so it is considered best practice to choose specific state types so as
// not to create accidental conflicts.
//
// For this reason, even though
// `using function_state = std::queue<char const *>` would suffice for our
// needs with `PushQueue` and `PopQueue`, we choose to create a wrapper type.
struct Queue {
  std::queue<char const *> queue;
};

struct PushQueue : hop::Instruction<PushQueue> {
  using function_state = Queue;
  static void execute(function_state &state, hop::Input<>, hop::Output<>,
                      char const *cstr) {
    state.queue.push(cstr);
  }
};

struct PopQueue : hop::Instruction<PopQueue> {
  using function_state = Queue;

  static void execute(function_state &state, hop::Input<>,
                      hop::Output<char const *> out) {
    out.set<0>(state.queue.front());
    state.queue.pop();
  }
};

struct RotateQueue : hop::Instruction<RotateQueue> {
  using function_state = Queue;
  static void execute(function_state &state, hop::Input<>,
                      hop::Output<>) {
    char const *top = state.queue.front();
    state.queue.pop();
    state.queue.push(top);
  }
};

void HelloWorld() {
  using Instructions = hop::MakeInstructionSet<PrintCString, PushQueue,
                                                  PopQueue, RotateQueue>;

  hop::Function<Instructions> func(0, 0);

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

  func.append<hop::Return>();

  // Now that our function has been defined, we can execute it.
  nth::stack<hop::Value> stack;
  func.invoke(stack);
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
