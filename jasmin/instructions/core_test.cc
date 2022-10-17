#include "jasmin/instructions/core.h"

#include "gtest/gtest.h"
#include "jasmin/testing.h"

namespace {

TEST(Instruction, Push) {
  jasmin::ValueStack value_stack;

  jasmin::ExecuteInstruction<jasmin::Push>(value_stack, 17);
  EXPECT_EQ(value_stack.size(), 1);
  EXPECT_EQ(value_stack.peek<int>(), 17);

  jasmin::ExecuteInstruction<jasmin::Push>(value_stack, 18);
  EXPECT_EQ(value_stack.size(), 2);
  EXPECT_EQ(value_stack.peek<int>(), 18);
}

TEST(Instruction, Duplicate) {
  jasmin::ValueStack value_stack;
  value_stack.push(3);

  jasmin::ExecuteInstruction<jasmin::Duplicate>(value_stack);
  EXPECT_EQ(value_stack.size(), 2);
  EXPECT_EQ(value_stack.peek<int>(), 3);

  jasmin::ExecuteInstruction<jasmin::Duplicate>(value_stack);
  EXPECT_EQ(value_stack.size(), 3);
  EXPECT_EQ(value_stack.peek<int>(), 3);
}

TEST(Instruction, Swap) {
  jasmin::ValueStack value_stack;
  value_stack.push(1);
  value_stack.push(2);

  jasmin::ExecuteInstruction<jasmin::Swap>(value_stack);
  EXPECT_EQ(value_stack.size(), 2);
  EXPECT_EQ(value_stack.pop<int>(), 1);
  EXPECT_EQ(value_stack.pop<int>(), 2);
}

TEST(Instruction, Load) {
  int8_t n8   = 8;
  int16_t n16 = 16;
  int32_t n32 = 32;
  int64_t n64 = 64;
  jasmin::ValueStack value_stack;

  value_stack.push(&n8);
  jasmin::ExecuteInstruction<jasmin::Load>(value_stack, sizeof(int8_t));

  value_stack.push(&n16);
  jasmin::ExecuteInstruction<jasmin::Load>(value_stack, sizeof(int16_t));

  value_stack.push(&n32);
  jasmin::ExecuteInstruction<jasmin::Load>(value_stack, sizeof(int32_t));

  value_stack.push(&n64);
  jasmin::ExecuteInstruction<jasmin::Load>(value_stack, sizeof(int64_t));

  EXPECT_EQ(value_stack.size(), 4);
  EXPECT_EQ(value_stack.pop<int8_t>(), 64);
  EXPECT_EQ(value_stack.pop<int8_t>(), 32);
  EXPECT_EQ(value_stack.pop<int8_t>(), 16);
  EXPECT_EQ(value_stack.pop<int8_t>(), 8);
}

}  // namespace
