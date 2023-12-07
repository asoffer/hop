#include "jasmin/instructions/bool.h"

#include "nth/test/test.h"
#include "jasmin/testing.h"
#include "nth/container/stack.h"

namespace {

NTH_TEST("instruction/Not") {
  nth::stack<jasmin::Value> value_stack;

  value_stack.push(true);
  jasmin::ExecuteInstruction<jasmin::Not>(value_stack);
  NTH_ASSERT(value_stack.size() == 1);
  NTH_EXPECT(not value_stack.top().as<bool>());

  jasmin::ExecuteInstruction<jasmin::Not>(value_stack);
  NTH_ASSERT(value_stack.size() == 1);
  NTH_EXPECT(value_stack.top().as<bool>());
}

NTH_TEST("instruction/Xor") {
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Xor>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(not value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Xor>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }

  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Xor>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Xor>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(not value_stack.top().as<bool>());
  }
}

NTH_TEST("instruction/And") {
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::And>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::And>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(not value_stack.top().as<bool>());
  }

  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::And>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(not value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::And>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(not value_stack.top().as<bool>());
  }
}

NTH_TEST("instruction/Or") {
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Or>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Or>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }

  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Or>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Or>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(not value_stack.top().as<bool>());
  }
}

NTH_TEST("instruction/Nand") {
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Nand>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(not value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(true);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Nand>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }

  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(true);
    jasmin::ExecuteInstruction<jasmin::Nand>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }
  {
    nth::stack<jasmin::Value> value_stack;
    value_stack.push(false);
    value_stack.push(false);
    jasmin::ExecuteInstruction<jasmin::Nand>(value_stack);
    NTH_ASSERT(value_stack.size() == 1);
    NTH_EXPECT(value_stack.top().as<bool>());
  }
}

}  // namespace
