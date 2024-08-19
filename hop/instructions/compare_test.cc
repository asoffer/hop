#include "hop/instructions/compare.h"

#include "hop/testing.h"
#include "nth/container/stack.h"
#include "nth/test/test.h"

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

NTH_TEST("Equal/Primitive") {
  nth::stack<hop::Value> value_stack;
  value_stack.push(3);
  value_stack.push(5);

  hop::ExecuteInstruction<hop::Equal<int>>(value_stack);
  NTH_ASSERT(value_stack.size() == 1);
  NTH_EXPECT(not value_stack.top().as<bool>());
  value_stack.pop();

  value_stack.push(4);
  value_stack.push(4);
  hop::ExecuteInstruction<hop::Equal<int>>(value_stack);
  NTH_ASSERT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<bool>());
  value_stack.pop();
}

NTH_TEST("Equal/UserDefined") {
  nth::stack<hop::Value> value_stack;

  value_stack.push(S{.value = 3});
  value_stack.push(S{.value = 5});
  hop::ExecuteInstruction<hop::Equal<S>>(value_stack);
  NTH_ASSERT(value_stack.size() == 1);
  NTH_EXPECT(not value_stack.top().as<bool>());
  value_stack.pop();

  value_stack.push(S{.value = 4});
  value_stack.push(S{.value = 5});
  hop::ExecuteInstruction<hop::Equal<S>>(value_stack);
  NTH_ASSERT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<bool>());
  value_stack.pop();
}

NTH_TEST("LessThan/Primitive") {
  nth::stack<hop::Value> value_stack;
  value_stack.push(3);
  value_stack.push(5);

  hop::ExecuteInstruction<hop::LessThan<int>>(value_stack);
  NTH_ASSERT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<bool>());
  value_stack.pop();

  value_stack.push(4);
  value_stack.push(4);
  hop::ExecuteInstruction<hop::LessThan<int>>(value_stack);
  NTH_ASSERT(value_stack.size() == 1);
  NTH_EXPECT(not value_stack.top().as<bool>());
  value_stack.pop();
}

NTH_TEST("LessThan/UserDefined") {
  nth::stack<hop::Value> value_stack;

  value_stack.push(S{.value = 3});
  value_stack.push(S{.value = 5});
  hop::ExecuteInstruction<hop::LessThan<S>>(value_stack);
  NTH_ASSERT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<bool>());
  value_stack.pop();

  value_stack.push(S{.value = 4});
  value_stack.push(S{.value = 5});
  hop::ExecuteInstruction<hop::LessThan<S>>(value_stack);
  NTH_ASSERT(value_stack.size() == 1);
  NTH_EXPECT(not value_stack.top().as<bool>());
  value_stack.pop();
}

NTH_TEST("AppendEqual/Primitive") {
  nth::stack<hop::Value> value_stack;
  value_stack.push(3);
  value_stack.push(5);

  hop::ExecuteInstruction<hop::AppendEqual<int>>(value_stack);
  NTH_ASSERT(value_stack.size() == 3);
  NTH_EXPECT(not value_stack.top().as<bool>());
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<int>() == 5);
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<int>() == 3);
  value_stack.pop();

  value_stack.push(4);
  value_stack.push(4);
  hop::ExecuteInstruction<hop::AppendEqual<int>>(value_stack);
  NTH_ASSERT(value_stack.size() == 3);
  NTH_EXPECT(value_stack.top().as<bool>());
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<int>() == 4);
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<int>() == 4);
}

NTH_TEST("AppendEqual/UserDefined") {
  nth::stack<hop::Value> value_stack;

  value_stack.push(S{.value = 3});
  value_stack.push(S{.value = 5});
  hop::ExecuteInstruction<hop::AppendEqual<S>>(value_stack);
  NTH_ASSERT(value_stack.size() == 3);
  NTH_EXPECT(not value_stack.top().as<bool>());
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<S>().value == 5);
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<S>().value == 3);
  value_stack.pop();

  value_stack.push(S{.value = 4});
  value_stack.push(S{.value = 5});
  hop::ExecuteInstruction<hop::AppendEqual<S>>(value_stack);
  NTH_ASSERT(value_stack.size() == 3);
  NTH_EXPECT(value_stack.top().as<bool>());
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<S>().value == 5);
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<S>().value == 4);
}

NTH_TEST("AppendLessThan/Primitive") {
  nth::stack<hop::Value> value_stack;
  value_stack.push(3);
  value_stack.push(5);

  hop::ExecuteInstruction<hop::AppendLessThan<int>>(value_stack);
  NTH_ASSERT(value_stack.size() == 3);
  NTH_EXPECT(value_stack.top().as<bool>());
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<int>() == 5);
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<int>() == 3);
  value_stack.pop();

  value_stack.push(4);
  value_stack.push(4);
  hop::ExecuteInstruction<hop::AppendLessThan<int>>(value_stack);
  NTH_ASSERT(value_stack.size() == 3);
  NTH_EXPECT(not value_stack.top().as<bool>());
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<int>() == 4);
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<int>() == 4);
}

NTH_TEST("AppendLessThan/UserDefined") {
  nth::stack<hop::Value> value_stack;

  value_stack.push(S{.value = 3});
  value_stack.push(S{.value = 5});
  hop::ExecuteInstruction<hop::AppendLessThan<S>>(value_stack);
  NTH_ASSERT(value_stack.size() == 3);
  NTH_EXPECT(value_stack.top().as<bool>());
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<S>().value == 5);
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<S>().value == 3);
  value_stack.pop();

  value_stack.push(S{.value = 4});
  value_stack.push(S{.value = 5});
  hop::ExecuteInstruction<hop::AppendLessThan<S>>(value_stack);
  NTH_ASSERT(value_stack.size() == 3);
  NTH_EXPECT(not value_stack.top().as<bool>());
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<S>().value == 5);
  value_stack.pop();
  NTH_EXPECT(value_stack.top().as<S>().value == 4);
}

}  // namespace
