#include "jasmin/internal/function_base.h"

#include "gtest/gtest.h"

namespace jasmin::internal_function_base {
namespace {

TEST(FunctionBase, Simple) {
  FunctionBase f(1, 2);
  EXPECT_EQ(f.parameter_count(), 1);
  EXPECT_EQ(f.return_count(), 2);
}

}  // namespace
}  // namespace jasmin::internal_function_base
