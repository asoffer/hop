#include "jasmin/instructions/arithmetic.h"

#include "jasmin/testing.h"
#include "nth/container/stack.h"
#include "nth/test/test.h"

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

NTH_TEST("Add/Primitive") {
  nth::stack<jasmin::Value> value_stack;
  value_stack.push(3);
  value_stack.push(5);
  jasmin::ExecuteInstruction<jasmin::Add<int>>(value_stack);
  NTH_EXPECT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<int>() == 8);
}

NTH_TEST("Add/UserDefined") {
  nth::stack<jasmin::Value> value_stack;
  value_stack.push(S{.value = 3});
  value_stack.push(S{.value = 5});
  jasmin::ExecuteInstruction<jasmin::Add<S>>(value_stack);
  NTH_EXPECT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<S>().value == 15);
}

NTH_TEST("Subtract/Primitive") {
  nth::stack<jasmin::Value> value_stack;
  value_stack.push(3);
  value_stack.push(5);
  jasmin::ExecuteInstruction<jasmin::Subtract<int>>(value_stack);
  NTH_EXPECT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<int>() == -2);
}

NTH_TEST("Subtract/UserDefined") {
  nth::stack<jasmin::Value> value_stack;
  value_stack.push(S{.value = 3});
  value_stack.push(S{.value = 5});
  jasmin::ExecuteInstruction<jasmin::Subtract<S>>(value_stack);
  NTH_EXPECT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<S>().value == 8);
}

NTH_TEST("Multiply/Primitive") {
  nth::stack<jasmin::Value> value_stack;
  value_stack.push(3);
  value_stack.push(5);
  jasmin::ExecuteInstruction<jasmin::Multiply<int>>(value_stack);
  NTH_EXPECT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<int>() == 15);
}

NTH_TEST("Multiply/UserDefined") {
  nth::stack<jasmin::Value> value_stack;
  value_stack.push(S{.value = 5});
  value_stack.push(S{.value = 3});
  jasmin::ExecuteInstruction<jasmin::Multiply<S>>(value_stack);
  NTH_EXPECT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<S>().value == 2);
}

NTH_TEST("Divide/Primitive") {
  nth::stack<jasmin::Value> value_stack;
  value_stack.push(6);
  value_stack.push(2);
  jasmin::ExecuteInstruction<jasmin::Divide<int>>(value_stack);
  NTH_EXPECT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<int>() == 3);
}

NTH_TEST("Divide/UserDefined") {
  nth::stack<jasmin::Value> value_stack;
  value_stack.push(S{.value = 6});
  value_stack.push(S{.value = 2});
  jasmin::ExecuteInstruction<jasmin::Divide<S>>(value_stack);
  NTH_EXPECT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<S>().value == 4);
}

NTH_TEST("Mod/Primitive") {
  nth::stack<jasmin::Value> value_stack;
  value_stack.push(6);
  value_stack.push(4);
  jasmin::ExecuteInstruction<jasmin::Mod<int>>(value_stack);
  NTH_EXPECT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<int>() == 2);
}

NTH_TEST("Mod/UserDefined") {
  nth::stack<jasmin::Value> value_stack;
  value_stack.push(S{.value = 6});
  value_stack.push(S{.value = 2});
  jasmin::ExecuteInstruction<jasmin::Mod<S>>(value_stack);
  NTH_EXPECT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<S>().value == 3);
}

NTH_TEST("Negate/Primitive") {
  nth::stack<jasmin::Value> value_stack;
  value_stack.push(6);
  jasmin::ExecuteInstruction<jasmin::Negate<int>>(value_stack);
  NTH_EXPECT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<int>() == -6);
}

NTH_TEST("Negate/UserDefined") {
  nth::stack<jasmin::Value> value_stack;
  value_stack.push(S{.value = 6});
  jasmin::ExecuteInstruction<jasmin::Negate<S>>(value_stack);
  NTH_EXPECT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<S>().value == 7);
}

}  // namespace
