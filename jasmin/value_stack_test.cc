#include "jasmin/value_stack.h"

#include <tuple>

#include "gtest/gtest.h"
#include "nth/debug/log/log.h"
#include "nth/debug/log/stderr_log_sink.h"

namespace jasmin {
namespace {

bool init = [] {
  nth::RegisterLogSink(nth::stderr_log_sink);
  return false;
}();

TEST(ValueStack, DefaultConstruction) {
  ValueStack value_stack;
  EXPECT_TRUE(value_stack.empty());
  EXPECT_EQ(value_stack.size(), 0);
}

TEST(ValueStack, InitializerListConstruction) {
  {
    ValueStack value_stack{};
    EXPECT_TRUE(value_stack.empty());
    EXPECT_EQ(value_stack.size(), 0);
  }

  {
    ValueStack value_stack{1, true};
    EXPECT_FALSE(value_stack.empty());
    EXPECT_EQ(value_stack.size(), 2);
  }

  {
    ValueStack value_stack{1, true, Value(1)};
    EXPECT_FALSE(value_stack.empty());
    EXPECT_EQ(value_stack.size(), 3);
  }
}

TEST(ValueStack, PushPop) {
  ValueStack value_stack;

  value_stack.push(3);
  EXPECT_FALSE(value_stack.empty());
  EXPECT_EQ(value_stack.size(), 1);
  EXPECT_EQ(value_stack.peek_value().as<int>(), 3);
  EXPECT_EQ(value_stack.peek<int>(), 3);

  value_stack.push(true);
  EXPECT_FALSE(value_stack.empty());
  EXPECT_EQ(value_stack.size(), 2);
  EXPECT_TRUE(value_stack.peek_value().as<bool>());

  EXPECT_TRUE(value_stack.pop_value().as<bool>());
  EXPECT_FALSE(value_stack.empty());
  EXPECT_EQ(value_stack.size(), 1);

  EXPECT_EQ(value_stack.pop_value().as<int>(), 3);
  EXPECT_TRUE(value_stack.empty());
  EXPECT_EQ(value_stack.size(), 0);

  if constexpr (internal::harden) {
    EXPECT_DEATH({ value_stack.pop_value(); }, "empty ValueStack");
    EXPECT_DEATH({ value_stack.pop<bool>(); }, "empty ValueStack");
    EXPECT_DEATH({ value_stack.pop<bool>(); }, "empty ValueStack");
  }
  value_stack.push(true);
  EXPECT_TRUE(value_stack.pop<bool>());

  value_stack.push(3);
  EXPECT_EQ(value_stack.pop<int>(), 3);

  if constexpr (internal::debug) {
    value_stack.push(3);
    EXPECT_DEATH({ value_stack.peek<bool>(); }, "Value type mismatch");
    EXPECT_DEATH({ value_stack.pop<bool>(); }, "Value type mismatch");
  }
  value_stack.push(Value(3));
  EXPECT_EQ(value_stack.pop<int>(), 3);

  if constexpr (internal::debug) {
    EXPECT_DEATH({ value_stack.peek<bool>(); }, "Value type mismatch");
    EXPECT_DEATH({ value_stack.pop<bool>(); }, "Value type mismatch");
  }
}

TEST(ValueStack, Erase) {
  {
    ValueStack value_stack{1, true, 3.14};
    value_stack.erase(1, 1);
    EXPECT_EQ(value_stack.size(), 3);
    EXPECT_EQ(value_stack.pop<double>(), 3.14);
    EXPECT_TRUE(value_stack.pop<bool>());
    EXPECT_EQ(value_stack.pop<int>(), 1);
    EXPECT_TRUE(value_stack.empty());
  }

  {
    ValueStack value_stack{1, true, 3.14};
    value_stack.erase(1, 2);
    EXPECT_EQ(value_stack.size(), 2);
    EXPECT_EQ(value_stack.pop<double>(), 3.14);
    EXPECT_EQ(value_stack.pop<int>(), 1);
    EXPECT_TRUE(value_stack.empty());
  }
  {
    ValueStack value_stack{1, true, 3.14};
    value_stack.erase(0, 2);
    EXPECT_EQ(value_stack.size(), 1);
    EXPECT_EQ(value_stack.pop<double>(), 3.14);
    EXPECT_TRUE(value_stack.empty());
  }
  {
    ValueStack value_stack{1, true, 3.14};
    value_stack.erase(0, 3);
    EXPECT_TRUE(value_stack.empty());
  }

  if constexpr (internal::harden) {
    ValueStack value_stack{1, true, 3.14};
    EXPECT_DEATH({ value_stack.erase(2, 1); }, "invalid range");
    EXPECT_DEATH({ value_stack.erase(0, 5); }, "too few elements");
  }
}

TEST(ValueStack, Begin) {
  ValueStack value_stack{1, true, 3.14};
  EXPECT_EQ((value_stack.begin() + 2)->as<double>(), 3.14);
  EXPECT_EQ((value_stack.begin() + 1)->as<bool>(), true);
  EXPECT_EQ((value_stack.begin())->as<int>(), 1);
  value_stack.pop_value();
  EXPECT_EQ((value_stack.begin() + 1)->as<bool>(), true);
  EXPECT_EQ((value_stack.begin())->as<int>(), 1);
}

TEST(ValueStack, End) {
  ValueStack value_stack{1, true, 3.14};
  EXPECT_EQ((value_stack.end() - 1)->as<double>(), 3.14);
  EXPECT_EQ((value_stack.end() - 2)->as<bool>(), true);
  EXPECT_EQ((value_stack.end() - 3)->as<int>(), 1);
  value_stack.pop_value();
  EXPECT_EQ((value_stack.end() - 1)->as<bool>(), true);
  EXPECT_EQ((value_stack.end() - 2)->as<int>(), 1);
}

TEST(ValueStack, Peek) {
  ValueStack value_stack{1, true, 3.14};
  EXPECT_EQ(value_stack.peek<double>(0), 3.14);
  EXPECT_EQ(value_stack.peek<bool>(1), true);
  EXPECT_EQ(value_stack.peek<int>(2), 1);
  value_stack.pop_value();
  EXPECT_EQ(value_stack.peek<bool>(0), true);
  EXPECT_EQ(value_stack.peek<int>(1), 1);
}

}  // namespace
}  // namespace jasmin
