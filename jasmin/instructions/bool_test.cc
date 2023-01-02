#include "jasmin/instructions/bool.h"

#include "gtest/gtest.h"
#include "jasmin/testing.h"

namespace {

TEST(Instruction, Not) {
  jasmin::ValueStack value_stack;

  value_stack.push(true);
  jasmin::ExecuteInstruction<jasmin::Not>(value_stack);
  ASSERT_EQ(value_stack.size(), 1);
  EXPECT_FALSE(value_stack.peek<bool>());

  jasmin::ExecuteInstruction<jasmin::Not>(value_stack);
  ASSERT_EQ(value_stack.size(), 1);
  EXPECT_TRUE(value_stack.peek<bool>());
}

TEST(Instruction, Xor) {
  {
    jasmin::ValueStack value_stack;
    value_stack.push(true);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Xor>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_FALSE(value_stack.peek<bool>());
  }
  {
    jasmin::ValueStack value_stack;
    value_stack.push(true);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Xor>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.peek<bool>());
  }

  {
    jasmin::ValueStack value_stack;
    value_stack.push(false);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Xor>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.peek<bool>());
  }
  {
    jasmin::ValueStack value_stack;
    value_stack.push(false);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Xor>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_FALSE(value_stack.peek<bool>());
  }
}

TEST(Instruction, And) {
  {
    jasmin::ValueStack value_stack;
    value_stack.push(true);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::And>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.peek<bool>());
  }
  {
    jasmin::ValueStack value_stack;
    value_stack.push(true);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::And>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_FALSE(value_stack.peek<bool>());
  }

  {
    jasmin::ValueStack value_stack;
    value_stack.push(false);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::And>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_FALSE(value_stack.peek<bool>());
  }
  {
    jasmin::ValueStack value_stack;
    value_stack.push(false);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::And>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_FALSE(value_stack.peek<bool>());
  }
}

TEST(Instruction, Or) {
  {
    jasmin::ValueStack value_stack;
    value_stack.push(true);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Or>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.peek<bool>());
  }
  {
    jasmin::ValueStack value_stack;
    value_stack.push(true);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Or>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.peek<bool>());
  }

  {
    jasmin::ValueStack value_stack;
    value_stack.push(false);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Or>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.peek<bool>());
  }
  {
    jasmin::ValueStack value_stack;
    value_stack.push(false);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Or>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_FALSE(value_stack.peek<bool>());
  }
}

TEST(Instruction, Nand) {
  {
    jasmin::ValueStack value_stack;
    value_stack.push(true);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Nand>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_FALSE(value_stack.peek<bool>());
  }
  {
    jasmin::ValueStack value_stack;
    value_stack.push(true);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Nand>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.peek<bool>());
  }

  {
    jasmin::ValueStack value_stack;
    value_stack.push(false);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Nand>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.peek<bool>());
  }
  {
    jasmin::ValueStack value_stack;
    value_stack.push(false);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Nand>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.peek<bool>());
  }
}


}  // namespace
