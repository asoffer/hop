#include "hop/instructions/bool.h"

#include "nth/test/test.h"
#include "hop/testing.h"
#include "nth/container/stack.h"

namespace {

NTH_TEST("instruction/Not") {
  nth::stack<hop::Value> value_stack;

  value_stack.push(true);
  hop::ExecuteInstruction<hop::Not>(value_stack);
  NTH_ASSERT(value_stack.size() == 1);
  NTH_EXPECT(not value_stack.top().as<bool>());

  hop::ExecuteInstruction<hop::Not>(value_stack);
  NTH_ASSERT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<bool>());
}

NTH_TEST("instruction/Xor") {
  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(true);
    value_stack.push(true);
    hop::ExecuteInstruction<hop::Xor>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(not value_stack.top().as<bool>());
  }
  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(true);
    value_stack.push(false);
    hop::ExecuteInstruction<hop::Xor>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }

  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(false);
    value_stack.push(true);
    hop::ExecuteInstruction<hop::Xor>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }
  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(false);
    value_stack.push(false);
    hop::ExecuteInstruction<hop::Xor>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(not value_stack.top().as<bool>());
  }
}

NTH_TEST("instruction/And") {
  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(true);
    value_stack.push(true);
    hop::ExecuteInstruction<hop::And>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }
  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(true);
    value_stack.push(false);
    hop::ExecuteInstruction<hop::And>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(not value_stack.top().as<bool>());
  }

  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(false);
    value_stack.push(true);
    hop::ExecuteInstruction<hop::And>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(not value_stack.top().as<bool>());
  }
  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(false);
    value_stack.push(false);
    hop::ExecuteInstruction<hop::And>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(not value_stack.top().as<bool>());
  }
}

NTH_TEST("instruction/Or") {
  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(true);
    value_stack.push(true);
    hop::ExecuteInstruction<hop::Or>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }
  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(true);
    value_stack.push(false);
    hop::ExecuteInstruction<hop::Or>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }

  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(false);
    value_stack.push(true);
    hop::ExecuteInstruction<hop::Or>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }
  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(false);
    value_stack.push(false);
    hop::ExecuteInstruction<hop::Or>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(not value_stack.top().as<bool>());
  }
}

NTH_TEST("instruction/Nand") {
  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(true);
    value_stack.push(true);
    hop::ExecuteInstruction<hop::Nand>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(not value_stack.top().as<bool>());
  }
  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(true);
    value_stack.push(false);
    hop::ExecuteInstruction<hop::Nand>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }

  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(false);
    value_stack.push(true);
    hop::ExecuteInstruction<hop::Nand>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }
  {
    nth::stack<hop::Value> value_stack;
    value_stack.push(false);
    value_stack.push(false);
    hop::ExecuteInstruction<hop::Nand>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }
}

}  // namespace
