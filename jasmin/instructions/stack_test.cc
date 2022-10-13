#include "jasmin/instructions/stack.h"

#include "gtest/gtest.h"
#include "jasmin/testing.h"

namespace {

TEST(Instruction, StackAllocate) {
  jasmin::ValueStack value_stack;

  jasmin::internal::StackFrame frame;

  ASSERT_EQ(frame.data(), nullptr);
  jasmin::ExecuteInstruction<jasmin::StackAllocate>(value_stack, frame, 10);
  EXPECT_EQ(value_stack.size(), 0);
  EXPECT_NE(frame.data(), nullptr);
}

TEST(Instruction, StackOffset) {
  jasmin::ValueStack value_stack;

  jasmin::internal::StackFrame frame;

  ASSERT_EQ(frame.data(), nullptr);
  jasmin::ExecuteInstruction<jasmin::StackAllocate>(value_stack, frame, 10);
  jasmin::ExecuteInstruction<jasmin::StackOffset>(value_stack, frame, 3);
  EXPECT_EQ(value_stack.size(), 1);
  EXPECT_EQ(value_stack.peek<std::byte*>(), frame.data() + 3);
}

}  // namespace
