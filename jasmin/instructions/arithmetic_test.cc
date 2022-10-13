#include "jasmin/instructions/arithmetic.h"

#include "gtest/gtest.h"
#include "jasmin/testing.h"

namespace {

// A struct that has strange operators so we can verify the operators are
// called.
struct S {
  friend S operator+(S lhs, S rhs) { return S{.value = lhs.value * rhs.value}; }
  friend S operator-(S lhs, S rhs) { return S{.value = lhs.value + rhs.value}; }
  friend S operator/(S lhs, S rhs) { return S{.value = lhs.value - rhs.value}; }
  friend S operator%(S lhs, S rhs) { return S{.value = lhs.value / rhs.value}; }
  friend S operator*(S lhs, S rhs) { return S{.value = lhs.value % rhs.value}; }
  S operator-() { return S{.value = value + 1}; }

  int value;
};


TEST(Add, Primitive) {
  jasmin::ValueStack value_stack;
  value_stack.push(3);
  value_stack.push(5);
  jasmin::ExecuteInstruction<jasmin::Add<int>>(value_stack);
  EXPECT_EQ(value_stack.size(), 1);
  EXPECT_EQ(value_stack.peek<int>(), 8);
}

TEST(Add, UserDefined) {
  jasmin::ValueStack value_stack;
  value_stack.push(S{.value = 3});
  value_stack.push(S{.value = 5});
  jasmin::ExecuteInstruction<jasmin::Add<S>>(value_stack);
  EXPECT_EQ(value_stack.size(), 1);
  EXPECT_EQ(value_stack.peek<S>().value, 15);
}

TEST(Subtract, Primitive) {
  jasmin::ValueStack value_stack;
  value_stack.push(3);
  value_stack.push(5);
  jasmin::ExecuteInstruction<jasmin::Subtract<int>>(value_stack);
  EXPECT_EQ(value_stack.size(), 1);
  EXPECT_EQ(value_stack.peek<int>(), -2);
}

TEST(Subtract, UserDefined) {
  jasmin::ValueStack value_stack;
  value_stack.push(S{.value = 3});
  value_stack.push(S{.value = 5});
  jasmin::ExecuteInstruction<jasmin::Subtract<S>>(value_stack);
  EXPECT_EQ(value_stack.size(), 1);
  EXPECT_EQ(value_stack.peek<S>().value, 8);
}

TEST(Multiply, Primitive) {
  jasmin::ValueStack value_stack;
  value_stack.push(3);
  value_stack.push(5);
  jasmin::ExecuteInstruction<jasmin::Multiply<int>>(value_stack);
  EXPECT_EQ(value_stack.size(), 1);
  EXPECT_EQ(value_stack.peek<int>(), 15);
}

TEST(Multiply, UserDefined) {
  jasmin::ValueStack value_stack;
  value_stack.push(S{.value = 5});
  value_stack.push(S{.value = 3});
  jasmin::ExecuteInstruction<jasmin::Multiply<S>>(value_stack);
  EXPECT_EQ(value_stack.size(), 1);
  EXPECT_EQ(value_stack.peek<S>().value, 2);
}

TEST(Divide, Primitive) {
  jasmin::ValueStack value_stack;
  value_stack.push(6);
  value_stack.push(2);
  jasmin::ExecuteInstruction<jasmin::Divide<int>>(value_stack);
  EXPECT_EQ(value_stack.size(), 1);
  EXPECT_EQ(value_stack.peek<int>(), 3);
}

TEST(Divide, UserDefined) {
  jasmin::ValueStack value_stack;
  value_stack.push(S{.value = 6});
  value_stack.push(S{.value = 2});
  jasmin::ExecuteInstruction<jasmin::Divide<S>>(value_stack);
  EXPECT_EQ(value_stack.size(), 1);
  EXPECT_EQ(value_stack.peek<S>().value, 4);
}

TEST(Mod, Primitive) {
  jasmin::ValueStack value_stack;
  value_stack.push(6);
  value_stack.push(4);
  jasmin::ExecuteInstruction<jasmin::Mod<int>>(value_stack);
  EXPECT_EQ(value_stack.size(), 1);
  EXPECT_EQ(value_stack.peek<int>(), 2);
}

TEST(Mod, UserDefined) {
  jasmin::ValueStack value_stack;
  value_stack.push(S{.value = 6});
  value_stack.push(S{.value = 2});
  jasmin::ExecuteInstruction<jasmin::Mod<S>>(value_stack);
  EXPECT_EQ(value_stack.size(), 1);
  EXPECT_EQ(value_stack.peek<S>().value, 3);
}

TEST(Negate, Primitive) {
  jasmin::ValueStack value_stack;
  value_stack.push(6);
  jasmin::ExecuteInstruction<jasmin::Negate<int>>(value_stack);
  EXPECT_EQ(value_stack.size(), 1);
  EXPECT_EQ(value_stack.peek<int>(), -6);
}

TEST(Negate, UserDefined) {
  jasmin::ValueStack value_stack;
  value_stack.push(S{.value = 6});
  jasmin::ExecuteInstruction<jasmin::Negate<S>>(value_stack);
  EXPECT_EQ(value_stack.size(), 1);
  EXPECT_EQ(value_stack.peek<S>().value, 7);
}

}  // namespace
