#include "jasmin/internal/function_base.h"

#include "gtest/gtest.h"


namespace jasmin::internal {
namespace {

TEST(FunctionBase, Simple) {
  FunctionBase f(1, 2);
  EXPECT_EQ(f.parameter_count(), 1);
  EXPECT_EQ(f.return_count(), 2);
  EXPECT_TRUE(f.raw_instructions().empty());
}

TEST(FunctionBase, RawAppend) {
  FunctionBase f(0, 0);
  f.raw_append(3);
  f.raw_append(true);
  ASSERT_EQ(f.raw_instructions().size(), 2);
  EXPECT_EQ(f.raw_instructions()[0].as<int>(), 3);
  EXPECT_EQ(f.raw_instructions()[1].as<bool>(), true);
}

}  // namespace
}  // namespace jasmin::internal
