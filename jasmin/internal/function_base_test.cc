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

}  // namespace
}  // namespace jasmin::internal
