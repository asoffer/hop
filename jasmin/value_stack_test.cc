#include "jasmin/value_stack.h"

#include <tuple>

#include "gtest/gtest.h"

namespace jasmin {
namespace {

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

#if defined(JASMIN_DEBUG)
  EXPECT_DEATH({ value_stack.pop_value(); }, "empty ValueStack");
  EXPECT_DEATH({ value_stack.pop<bool>(); }, "empty ValueStack");
  EXPECT_DEATH({ value_stack.pop<bool>(); }, "empty ValueStack");
#endif  // defined(JASMIN_DEBUG)

  value_stack.push(true);
  EXPECT_TRUE(value_stack.pop<bool>());

  value_stack.push(3);
  EXPECT_EQ(value_stack.pop<int>(), 3);

#if defined(JASMIN_DEBUG)
  value_stack.push(3);
  EXPECT_DEATH({ value_stack.peek<bool>(); }, "Value type mismatch");
  EXPECT_DEATH({ value_stack.pop<bool>(); }, "Value type mismatch");
#endif  // defined(JASMIN_DEBUG)

  value_stack.push(Value(3));
  EXPECT_EQ(value_stack.pop<int>(), 3);

#if defined(JASMIN_DEBUG)
  EXPECT_DEATH({ value_stack.peek<bool>(); }, "Value type mismatch");
  EXPECT_DEATH({ value_stack.pop<bool>(); }, "Value type mismatch");
#endif  // defined(JASMIN_DEBUG)
}

TEST(ValueStack, SwapWith) {
  ValueStack value_stack;
#if defined(JASMIN_DEBUG)
  EXPECT_DEATH({ value_stack.swap_with(1); }, "too few elements");
#endif  // defined(JASMIN_DEBUG)
  value_stack.push(1);
#if defined(JASMIN_DEBUG)
  EXPECT_DEATH({ value_stack.swap_with(0); }, "with itself");
  EXPECT_DEATH({ value_stack.swap_with(1); }, "too few elements");
#endif  // defined(JASMIN_DEBUG)

  value_stack.push(true);
  value_stack.swap_with(1);
  EXPECT_EQ(value_stack.peek<int>(), 1);

  value_stack.swap_with(1);
  EXPECT_TRUE(value_stack.peek<bool>());

  value_stack.push(3.14);
  value_stack.swap_with(2);
  EXPECT_EQ(value_stack.peek<int>(), 1);

  value_stack.swap_with(2);
  EXPECT_EQ(value_stack.peek<double>(), 3.14);
}

TEST(ValueStack, PopSuffix) {
  {
    ValueStack value_stack{1, true, 3.14};
    EXPECT_EQ(value_stack.pop_suffix(), std::tuple());
  }
  {
    ValueStack value_stack{1, true, 3.14};
    EXPECT_EQ((value_stack.pop_suffix<bool, double>()), std::tuple(true, 3.14));
  }
#if defined(JASMIN_DEBUG)
  {
    ValueStack value_stack{1, true, 3.14};
    EXPECT_DEATH({ (value_stack.pop_suffix<bool, int>()); },
                 "Value type mismatch");
  }

  {
    ValueStack value_stack{1, true, 3.14};
    EXPECT_DEATH({ (value_stack.pop_suffix<int, int, bool, double>()); },
                 "too few elements");
  }
#endif  // defined(JASMIN_DEBUG)
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

#if defined(JASMIN_DEBUG)
  {
    ValueStack value_stack{1, true, 3.14};
    EXPECT_DEATH({ value_stack.erase(2, 1); }, "invalid range");
    EXPECT_DEATH({ value_stack.erase(0, 5); }, "too few elements");
  }
#endif  // defined(JASMIN_DEBUG)
}

}  // namespace
}  // namespace jasmin
