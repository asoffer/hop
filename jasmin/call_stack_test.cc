#include "jasmin/call_stack.h"

#include "gtest/gtest.h"

namespace jasmin {
namespace {

TEST(CallStack, Construction) {
  CallStack call_stack;
  EXPECT_TRUE(call_stack.empty());
#if defined(JASMIN_DEBUG)
  EXPECT_DEATH({ call_stack.current(); }, "empty call stack");
  EXPECT_DEATH({ call_stack.pop(); }, "empty call stack");
#endif  // defined(JASMIN_DEBUG)
}

TEST(CallStack, PushPop) {
  internal_function_base::FunctionBase f1(0, 0);
  internal_function_base::FunctionBase f2(1, 2);
  CallStack call_stack;
  call_stack.push(&f1, 10, f1.entry());
  EXPECT_FALSE(call_stack.empty());
  EXPECT_EQ(call_stack.current(), &f1);
  call_stack.push(&f2, 20, f2.entry());
  EXPECT_FALSE(call_stack.empty());
  EXPECT_EQ(call_stack.current(), &f2);

  call_stack.push(&f2, 21, f2.entry());
  EXPECT_FALSE(call_stack.empty());
  EXPECT_EQ(call_stack.current(), &f2);

#if defined(JASMIN_DEBUG)
  EXPECT_DEATH({ call_stack.push(&f2, 21, f2.entry()); }, "Value stack invariant broken");
  EXPECT_DEATH({ call_stack.push(&f2, 20, f2.entry()); }, "Value stack invariant broken");
  EXPECT_DEATH({ call_stack.push(&f2, 0, f2.entry()); }, "Value stack invariant broken");
  EXPECT_DEATH({ call_stack.push(&f1, 20, f1.entry()); }, "Value stack invariant broken");
#endif  // defined(JASMIN_DEBUG)

  EXPECT_EQ(call_stack.pop(), f2.entry());

  EXPECT_EQ(call_stack.pop(), f2.entry());
  EXPECT_FALSE(call_stack.empty());

  EXPECT_EQ(call_stack.pop(), f1.entry());
  EXPECT_TRUE(call_stack.empty());
}

TEST(CallStack, ErasableRange) {
  {
    CallStack call_stack;
    internal_function_base::FunctionBase f(0, 0);
    call_stack.push(&f, 0, f.entry());
    EXPECT_EQ(call_stack.erasable_range(0),
              (CallStack::ErasableRange{.start_index = 0, .end_index = 0}));
    EXPECT_EQ(call_stack.erasable_range(5),
              (CallStack::ErasableRange{.start_index = 0, .end_index = 5}));

    call_stack.push(&f, 10, f.entry());
    EXPECT_EQ(call_stack.erasable_range(10),
              (CallStack::ErasableRange{.start_index = 10, .end_index = 10}));
    EXPECT_EQ(call_stack.erasable_range(15),
              (CallStack::ErasableRange{.start_index = 10, .end_index = 15}));
  }

  {
    CallStack call_stack;
    internal_function_base::FunctionBase f(0, 5);
    call_stack.push(&f, 0, f.entry());
#if defined(JASMIN_DEBUG)
    EXPECT_DEATH({ call_stack.erasable_range(0); }, "frame clobbered");
#endif  // defined(JASMIN_DEBUG)
    EXPECT_EQ(call_stack.erasable_range(5),
              (CallStack::ErasableRange{.start_index = 0, .end_index = 0}));
    EXPECT_EQ(call_stack.erasable_range(10),
              (CallStack::ErasableRange{.start_index = 0, .end_index = 5}));

    call_stack.push(&f, 10, f.entry());
#if defined(JASMIN_DEBUG)
    EXPECT_DEATH({ call_stack.erasable_range(10); }, "frame clobbered");
#endif  // defined(JASMIN_DEBUG)
    EXPECT_EQ(call_stack.erasable_range(20),
              (CallStack::ErasableRange{.start_index = 10, .end_index = 15}));
    EXPECT_EQ(call_stack.erasable_range(25),
              (CallStack::ErasableRange{.start_index = 10, .end_index = 20}));
  }

  {
    CallStack call_stack;
    internal_function_base::FunctionBase f(5, 0);
    call_stack.push(&f, 6, f.entry());
#if defined(JASMIN_DEBUG)
    EXPECT_DEATH({ call_stack.erasable_range(0); }, "frame clobbered");
#endif  // defined(JASMIN_DEBUG)
    EXPECT_EQ(call_stack.erasable_range(5),
              (CallStack::ErasableRange{.start_index = 1, .end_index = 5}));
    EXPECT_EQ(call_stack.erasable_range(10),
              (CallStack::ErasableRange{.start_index = 1, .end_index = 10}));

    call_stack.push(&f, 12, f.entry());
    EXPECT_EQ(call_stack.erasable_range(7),
              (CallStack::ErasableRange{.start_index = 7, .end_index = 7}));
    EXPECT_EQ(call_stack.erasable_range(20),
              (CallStack::ErasableRange{.start_index = 7, .end_index = 20}));
    EXPECT_EQ(call_stack.erasable_range(25),
              (CallStack::ErasableRange{.start_index = 7, .end_index = 25}));
  }

  {
    CallStack call_stack;
    internal_function_base::FunctionBase f(3, 8);
    call_stack.push(&f, 5, f.entry());
#if defined(JASMIN_DEBUG)
    EXPECT_DEATH({ call_stack.erasable_range(9); }, "frame clobbered");
#endif  // defined(JASMIN_DEBUG)
    EXPECT_EQ(call_stack.erasable_range(10),
              (CallStack::ErasableRange{.start_index = 2, .end_index = 2}));
    EXPECT_EQ(call_stack.erasable_range(20),
              (CallStack::ErasableRange{.start_index = 2, .end_index = 12}));

    call_stack.push(&f, 12, f.entry());
    EXPECT_EQ(call_stack.erasable_range(17),
              (CallStack::ErasableRange{.start_index = 9, .end_index = 9}));
    EXPECT_EQ(call_stack.erasable_range(20),
              (CallStack::ErasableRange{.start_index = 9, .end_index = 12}));
  }
}

}  // namespace
}  // namespace jasmin
