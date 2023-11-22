#include "instruction_traits.h"

#include "gtest/gtest.h"

namespace jasmin::internal {
namespace {

struct Count : StackMachineInstruction<Count> {
  using function_state                    = int;
  static constexpr size_t parameter_count = 0;
  static constexpr size_t return_count    = 1;
  static void execute(ValueStack& v, function_state& state) { v.push(state++); }
};

struct NoImmediates : StackMachineInstruction<NoImmediates> {
  static int execute(int, int) { return 0; }
};

struct EmptyImmediates : StackMachineInstruction<EmptyImmediates> {
  static constexpr size_t parameter_count = 0;
  static constexpr size_t return_count    = 0;
  static void execute(ValueStack&) {}
};

struct SomeImmediates : StackMachineInstruction<SomeImmediates> {
  static constexpr size_t parameter_count = 0;
  static constexpr size_t return_count    = 0;
  static void execute(ValueStack&, int, bool) {}
};

TEST(ImmediateValueCount, Value) {
  EXPECT_EQ(ImmediateValueCount<Return>(), 0);
  EXPECT_EQ(ImmediateValueCount<Call>(), 0);
  EXPECT_EQ(ImmediateValueCount<Jump>(), 1);
  EXPECT_EQ(ImmediateValueCount<JumpIf>(), 1);
  EXPECT_EQ(ImmediateValueCount<Count>(), 0);
  EXPECT_EQ(ImmediateValueCount<NoImmediates>(), 0);
  EXPECT_EQ(ImmediateValueCount<EmptyImmediates>(), 0);
  EXPECT_EQ(ImmediateValueCount<SomeImmediates>(), 2);
}

TEST(ParameterCount, Value) {
  EXPECT_EQ(ParameterCount<Return>(), 0);
  EXPECT_EQ(ParameterCount<Call>(), 1);
  EXPECT_EQ(ParameterCount<Jump>(), 0);
  EXPECT_EQ(ParameterCount<JumpIf>(), 1);
  EXPECT_EQ(ParameterCount<Count>(), 0);
  EXPECT_EQ(ParameterCount<NoImmediates>(), 2);
  EXPECT_EQ(ParameterCount<EmptyImmediates>(), 0);
  EXPECT_EQ(ParameterCount<SomeImmediates>(), 0);
}

TEST(ReturnCount, Value) {
  EXPECT_EQ(ReturnCount<Return>(), 0);
  EXPECT_EQ(ReturnCount<Call>(), 0);
  EXPECT_EQ(ReturnCount<Jump>(), 0);
  EXPECT_EQ(ReturnCount<JumpIf>(), 0);
  EXPECT_EQ(ReturnCount<Count>(), 1);
  EXPECT_EQ(ReturnCount<NoImmediates>(), 1);
  EXPECT_EQ(ReturnCount<EmptyImmediates>(), 0);
  EXPECT_EQ(ReturnCount<SomeImmediates>(), 0);
}

}  // namespace
}  // namespace jasmin::internal
