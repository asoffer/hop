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
  EXPECT_EQ(value_stack.pop<int64_t>(), 64);
  EXPECT_EQ(value_stack.pop<int32_t>(), 32);
  EXPECT_EQ(value_stack.pop<int16_t>(), 16);
  EXPECT_EQ(value_stack.pop<int8_t>(), 8);
}

TEST(Instruction, StoreConstant) {
  int8_t n8   = 0;
  int16_t n16 = 0;
  int32_t n32 = 0;
  int64_t n64 = 0;
  jasmin::ValueStack value_stack;

  value_stack.push(int8_t{8});
  jasmin::ExecuteInstruction<jasmin::StoreConstant>(value_stack, &n8,
                                                    sizeof(int8_t));
  value_stack.push(int16_t{16});
  jasmin::ExecuteInstruction<jasmin::StoreConstant>(value_stack, &n16,
                                                    sizeof(int16_t));
  value_stack.push(int32_t{32});
  jasmin::ExecuteInstruction<jasmin::StoreConstant>(value_stack, &n32,
                                                    sizeof(int32_t));
  value_stack.push(int64_t{64});
  jasmin::ExecuteInstruction<jasmin::StoreConstant>(value_stack, &n64,
                                                    sizeof(int64_t));

  EXPECT_EQ(value_stack.size(), 0);
  EXPECT_EQ(n64, 64);
  EXPECT_EQ(n32, 32);
  EXPECT_EQ(n16, 16);
  EXPECT_EQ(n8, 8);
}

}  // namespace
