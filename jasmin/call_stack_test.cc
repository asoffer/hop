#include "jasmin/call_stack.h"

#include "gtest/gtest.h"
#include "nth/debug/log/log.h"
#include "nth/debug/log/stderr_log_sink.h"

namespace jasmin {
namespace {

bool init = [] {
  nth::RegisterLogSink(nth::stderr_log_sink);
  return false;
}();

TEST(CallStack, Construction) {
  CallStack call_stack;
  EXPECT_TRUE(call_stack.empty());
#if defined(JASMIN_DEBUG)
  EXPECT_DEATH({ call_stack.current(); }, "empty call stack");
  EXPECT_DEATH({ call_stack.pop(); }, "empty call stack");
#endif  // defined(JASMIN_DEBUG)
}

TEST(CallStack, PushPop) {
  internal::FunctionBase f1(0, 0);
  internal::FunctionBase f2(1, 2);
  CallStack call_stack;
  call_stack.push(&f1, f1.entry());
  EXPECT_FALSE(call_stack.empty());
  EXPECT_EQ(call_stack.current(), &f1);
  call_stack.push(&f2, f2.entry());
  EXPECT_FALSE(call_stack.empty());
  EXPECT_EQ(call_stack.current(), &f2);

  call_stack.push(&f2, f2.entry());
  EXPECT_FALSE(call_stack.empty());
  EXPECT_EQ(call_stack.current(), &f2);

  EXPECT_EQ(call_stack.pop(), f2.entry());

  EXPECT_EQ(call_stack.pop(), f2.entry());
  EXPECT_FALSE(call_stack.empty());

  EXPECT_EQ(call_stack.pop(), f1.entry());
  EXPECT_TRUE(call_stack.empty());
}

}  // namespace
}  // namespace jasmin
