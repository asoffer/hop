#include "jasmin/instruction_pointer.h"

#include "gtest/gtest.h"

namespace jasmin {
namespace {

TEST(InstructionPointer, Comparison) {
  std::vector<OpCodeOrValue> v;
  for (int i = 0; i < 10; ++i) { v.push_back(OpCodeOrValue::Value(i)); }


  InstructionPointer p(&v[5]);
  InstructionPointer q(&v[6]);
  EXPECT_FALSE(p == q);
  EXPECT_TRUE(p != q);
  EXPECT_TRUE(p + 1 == q);
  EXPECT_FALSE(p + 1 != q);
  ++p;
  EXPECT_TRUE(p == q);
  EXPECT_FALSE(p != q);
}

TEST(InstructionPointer, Access) {
  std::vector<OpCodeOrValue> v;
  for (int i = 0; i < 10; ++i) { v.push_back(OpCodeOrValue::Value(i)); }

  InstructionPointer p(&v[5]);
  EXPECT_EQ(p->value().as<int>(), 5);

  EXPECT_EQ((++p)->value().as<int>(), 6);
  EXPECT_EQ(p->value().as<int>(), 6);

  EXPECT_EQ((--p)->value().as<int>(), 5);
  EXPECT_EQ(p->value().as<int>(), 5);

  EXPECT_EQ((p++)->value().as<int>(), 5);
  EXPECT_EQ(p->value().as<int>(), 6);

  EXPECT_EQ((p--)->value().as<int>(), 6);
  EXPECT_EQ(p->value().as<int>(), 5);

  EXPECT_EQ(&(p += 2), &p);
  EXPECT_EQ(p->value().as<int>(), 7);
  EXPECT_EQ(&(p -= 3), &p);
  EXPECT_EQ(p->value().as<int>(), 4);

  EXPECT_EQ((p + 1)->value().as<int>(), 5);
  EXPECT_EQ((1 + p)->value().as<int>(), 5);
  EXPECT_EQ((p - 1)->value().as<int>(), 3);
}

}  // namespace
}  // namespace jasmin
