#include "jasmin/instructions/compare.h"

#include "gtest/gtest.h"
#include "jasmin/testing.h"

namespace {

// A struct that has strange operators so we can verify the operators are
// called.
struct S {
  friend bool operator==(S lhs, S rhs) {
    return lhs.value / 2 == rhs.value / 2;
  }
  friend bool operator<(S lhs, S rhs) { return lhs.value / 2 < rhs.value / 2; }

  int value;
};

TEST(Equal, Primitive) {
  jasmin::ValueStack value_stack;
  value_stack.push(3);
  value_stack.push(5);

  jasmin::ExecuteInstruction<jasmin::Equal<int>>(value_stack);
  ASSERT_EQ(value_stack.size(), 1);
  EXPECT_FALSE(value_stack.pop<bool>());

  value_stack.push(4);
  value_stack.push(4);
  jasmin::ExecuteInstruction<jasmin::Equal<int>>(value_stack);
  ASSERT_EQ(value_stack.size(), 1);
  EXPECT_TRUE(value_stack.pop<bool>());
}

TEST(Equal, UserDefined) {
  jasmin::ValueStack value_stack;

  value_stack.push(S{.value = 3});
  value_stack.push(S{.value = 5});
  jasmin::ExecuteInstruction<jasmin::Equal<S>>(value_stack);
  ASSERT_EQ(value_stack.size(), 1);
  EXPECT_FALSE(value_stack.pop<bool>());

  value_stack.push(S{.value = 4});
  value_stack.push(S{.value = 5});
  jasmin::ExecuteInstruction<jasmin::Equal<S>>(value_stack);
  ASSERT_EQ(value_stack.size(), 1);
  EXPECT_TRUE(value_stack.pop<bool>());
}

TEST(LessThan, Primitive) {
  jasmin::ValueStack value_stack;
  value_stack.push(3);
  value_stack.push(5);

  jasmin::ExecuteInstruction<jasmin::LessThan<int>>(value_stack);
  ASSERT_EQ(value_stack.size(), 1);
  EXPECT_TRUE(value_stack.pop<bool>());

  value_stack.push(4);
  value_stack.push(4);
  jasmin::ExecuteInstruction<jasmin::LessThan<int>>(value_stack);
  ASSERT_EQ(value_stack.size(), 1);
  EXPECT_FALSE(value_stack.pop<bool>());
}

TEST(LessThan, UserDefined) {
  jasmin::ValueStack value_stack;

  value_stack.push(S{.value = 3});
  value_stack.push(S{.value = 5});
  jasmin::ExecuteInstruction<jasmin::LessThan<S>>(value_stack);
  ASSERT_EQ(value_stack.size(), 1);
  EXPECT_TRUE(value_stack.pop<bool>());

  value_stack.push(S{.value = 4});
  value_stack.push(S{.value = 5});
  jasmin::ExecuteInstruction<jasmin::LessThan<S>>(value_stack);
  ASSERT_EQ(value_stack.size(), 1);
  EXPECT_FALSE(value_stack.pop<bool>());
}

TEST(AppendEqual, Primitive) {
  jasmin::ValueStack value_stack;
  value_stack.push(3);
  value_stack.push(5);

  jasmin::ExecuteInstruction<jasmin::AppendEqual<int>>(value_stack);
  ASSERT_EQ(value_stack.size(), 3);
  EXPECT_FALSE(value_stack.pop<bool>());
  EXPECT_EQ(value_stack.pop<int>(), 5);
  EXPECT_EQ(value_stack.pop<int>(), 3);

  value_stack.push(4);
  value_stack.push(4);
  jasmin::ExecuteInstruction<jasmin::AppendEqual<int>>(value_stack);
  ASSERT_EQ(value_stack.size(), 3);
  EXPECT_TRUE(value_stack.pop<bool>());
  EXPECT_EQ(value_stack.pop<int>(), 4);
  EXPECT_EQ(value_stack.pop<int>(), 4);
}

TEST(AppendEqual, UserDefined) {
  jasmin::ValueStack value_stack;

  value_stack.push(S{.value = 3});
  value_stack.push(S{.value = 5});
  jasmin::ExecuteInstruction<jasmin::AppendEqual<S>>(value_stack);
  ASSERT_EQ(value_stack.size(), 3);
  EXPECT_FALSE(value_stack.pop<bool>());
  EXPECT_EQ(value_stack.pop<S>().value, 5);
  EXPECT_EQ(value_stack.pop<S>().value, 3);

  value_stack.push(S{.value = 4});
  value_stack.push(S{.value = 5});
  jasmin::ExecuteInstruction<jasmin::AppendEqual<S>>(value_stack);
  ASSERT_EQ(value_stack.size(), 3);
  EXPECT_TRUE(value_stack.pop<bool>());
  EXPECT_EQ(value_stack.pop<S>().value, 5);
  EXPECT_EQ(value_stack.pop<S>().value, 4);
}

TEST(AppendLessThan, Primitive) {
  jasmin::ValueStack value_stack;
  value_stack.push(3);
  value_stack.push(5);

  jasmin::ExecuteInstruction<jasmin::AppendLessThan<int>>(value_stack);
  ASSERT_EQ(value_stack.size(), 3);
  EXPECT_TRUE(value_stack.pop<bool>());
  EXPECT_EQ(value_stack.pop<int>(), 5);
  EXPECT_EQ(value_stack.pop<int>(), 3);

  value_stack.push(4);
  value_stack.push(4);
  jasmin::ExecuteInstruction<jasmin::AppendLessThan<int>>(value_stack);
  ASSERT_EQ(value_stack.size(), 3);
  EXPECT_FALSE(value_stack.pop<bool>());
  EXPECT_EQ(value_stack.pop<int>(), 4);
  EXPECT_EQ(value_stack.pop<int>(), 4);
}

TEST(AppendLessThan, UserDefined) {
  jasmin::ValueStack value_stack;

  value_stack.push(S{.value = 3});
  value_stack.push(S{.value = 5});
  jasmin::ExecuteInstruction<jasmin::AppendLessThan<S>>(value_stack);
  ASSERT_EQ(value_stack.size(), 3);
  EXPECT_TRUE(value_stack.pop<bool>());
  EXPECT_EQ(value_stack.pop<S>().value, 5);
  EXPECT_EQ(value_stack.pop<S>().value, 3);

  value_stack.push(S{.value = 4});
  value_stack.push(S{.value = 5});
  jasmin::ExecuteInstruction<jasmin::AppendLessThan<S>>(value_stack);
  ASSERT_EQ(value_stack.size(), 3);
  EXPECT_FALSE(value_stack.pop<bool>());
  EXPECT_EQ(value_stack.pop<S>().value, 5);
  EXPECT_EQ(value_stack.pop<S>().value, 4);
}

}  // namespace
