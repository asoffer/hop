#include "hop/instructions/stack.h"

#include "hop/testing.h"
#include "nth/container/stack.h"
#include "nth/test/test.h"

namespace {

NTH_TEST("instruction/StackAllocate") {
  nth::stack<hop::Value> value_stack;

  hop::internal::StackFrame frame;

  NTH_ASSERT(frame.data() == nullptr);
  hop::ExecuteInstruction<hop::StackAllocate>(value_stack, frame, 10);
  NTH_EXPECT(value_stack.size() == 0);
  NTH_EXPECT(frame.data() != nullptr);
}

NTH_TEST("instruction/StackOffset") {
  nth::stack<hop::Value> value_stack;

  hop::internal::StackFrame frame;

  NTH_ASSERT(frame.data() == nullptr);
  hop::ExecuteInstruction<hop::StackAllocate>(value_stack, frame, 10);
  hop::ExecuteInstruction<hop::StackOffset>(value_stack, frame, 3);
  NTH_EXPECT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<std::byte*>() == frame.data() + 3);
}

}  // namespace
