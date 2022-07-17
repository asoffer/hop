#include "jasmin/function.h"

#include "gtest/gtest.h"

namespace jasmin {
namespace {

template <typename... Arguments>
struct MockFunction : StackMachineInstruction<MockFunction<Arguments...>> {
  static constexpr void execute(ValueStack&, Arguments...) {};
};

TEST(Function, AppendNoArguments) {
  using instruction_set = MakeInstructionSet<MockFunction<>>;
  Function<instruction_set> f(0, 0);
  f.append<MockFunction<>>();
  EXPECT_EQ(f.size(), 1);
}

TEST(Function, AppendWithArguments) {
  using instruction_set = MakeInstructionSet<MockFunction<int, bool>>;
  Function<instruction_set> f(0, 0);
  f.append<MockFunction<int, bool>>(3, true);
  EXPECT_EQ(f.size(), 3);
  EXPECT_EQ((f.append_with_placeholders<MockFunction<int, bool>>()), 6);
  EXPECT_EQ(f.size(), 6);
}

}  // namespace
}  // namespace jasmin
