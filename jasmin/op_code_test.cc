#include "jasmin/op_code.h"

#include "gtest/gtest.h"

namespace jasmin {
namespace {

TEST(OpCodeOrValue, OpCodeOrValue) {
  auto op_code = OpCodeOrValue::OpCode(1);
  auto value = OpCodeOrValue::Value(2);
  EXPECT_EQ(op_code.op_code(), 1);
  EXPECT_EQ(value.value().as<int>(), 2);
#if defined(JASMIN_DEBUG)
  EXPECT_DEATH({ op_code.value(); }, "holds an op-code");
  EXPECT_DEATH({ value.op_code(); }, "holds a Value");
#endif  // defined(JASMIN_DEBUG)

  value.set_value(true);
  EXPECT_TRUE(value.value().as<bool>());
#if defined(JASMIN_DEBUG)
  EXPECT_DEATH({ op_code.set_value(3); }, "holds an op-code");
#endif  // defined(JASMIN_DEBUG)
  
 
  auto uninit = OpCodeOrValue::UninitializedValue();
  uninit.set_value(true);
  EXPECT_TRUE(uninit.value().as<bool>());
}

// TODO: Improve test coverage.

}  // namespace
}  // namespace jasmin
