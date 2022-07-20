#include "jasmin/op_code.h"

#include "gtest/gtest.h"

namespace jasmin {
namespace {

TEST(OpCodeRange, OpCodeRange) {
  {
    OpCodeRange r1(3, 5);
    OpCodeRange r2(10, 2);
    EXPECT_EQ(OpCodeRange::Distance(r2, r1), 7);
    EXPECT_EQ(OpCodeRange::Distance(r1, r2), -7);

    EXPECT_EQ(r1.offset(), 3);
    EXPECT_EQ(r1.size(), 5);

    EXPECT_EQ(r2.offset(), 10);
    EXPECT_EQ(r2.size(), 2);
  }

  {
    OpCodeRange r1(3, 5);
    OpCodeRange r2(3, 2);
    EXPECT_EQ(OpCodeRange::Distance(r2, r1), 0);
    EXPECT_EQ(OpCodeRange::Distance(r1, r2), 0);

    EXPECT_EQ(r1.offset(), 3);
    EXPECT_EQ(r1.size(), 5);

    EXPECT_EQ(r2.offset(), 3);
    EXPECT_EQ(r2.size(), 2);
  }
}

}  // namespace
}  // namespace jasmin
