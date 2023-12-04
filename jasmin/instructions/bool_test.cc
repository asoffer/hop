#include "jasmin/instructions/bool.h"

#include "gtest/gtest.h"
#include "jasmin/testing.h"
#include "nth/container/stack.h"

namespace {

TEST(Instruction, Not) {
  nth::stack<jasmin::Value> value_stack;

  value_stack.push(true);
  jasmin::ExecuteInstruction<jasmin::Not>(value_stack);
  ASSERT_EQ(value_stack.size(), 1);
  EXPECT_FALSE(value_stack.top().as<bool>());

  jasmin::ExecuteInstruction<jasmin::Not>(value_stack);
  ASSERT_EQ(value_stack.size(), 1);
  EXPECT_TRUE(value_stack.top().as<bool>());
}

TEST(Instruction, Xor) {
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Xor>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_FALSE(value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Xor>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.top().as<bool>());
  }

  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Xor>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Xor>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_FALSE(value_stack.top().as<bool>());
  }
}

TEST(Instruction, And) {
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::And>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::And>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_FALSE(value_stack.top().as<bool>());
  }

  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::And>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_FALSE(value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::And>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_FALSE(value_stack.top().as<bool>());
  }
}

TEST(Instruction, Or) {
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Or>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Or>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.top().as<bool>());
  }

  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Or>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Or>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_FALSE(value_stack.top().as<bool>());
  }
}

TEST(Instruction, Nand) {
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Nand>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_FALSE(value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Nand>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.top().as<bool>());
  }

  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Nand>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Nand>(value_stack);
    ASSERT_EQ(value_stack.size(), 1);
    EXPECT_TRUE(value_stack.top().as<bool>());
  }
}

}  // namespace
