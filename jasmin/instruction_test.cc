#include "jasmin/instruction.h"

#include "gtest/gtest.h"

namespace jasmin {
namespace {

struct NoOp : StackMachineInstruction<NoOp> {
  static void execute() {}
};
struct Duplicate : StackMachineInstruction<Duplicate> {
  static void execute(ValueStack& v) { v.push(v.peek_value()); }
};
struct PushOne : StackMachineInstruction<PushOne> {
  static void execute(ValueStack& v) { v.push(1); }
};

using Set = MakeInstructionSet<NoOp, Duplicate>;

template <Instruction I>
size_t CountInstructionMatch() {
  size_t count = 0;
  for (size_t i = 0; i < Set::size(); ++i) {
    if (Set::InstructionFunction(i) == &I::template ExecuteImpl<Set>) {
      ++count;
    }
  }
  return count;
}

TEST(Instruction, Construction) {
  EXPECT_EQ(CountInstructionMatch<NoOp>(), 1);
  EXPECT_EQ(CountInstructionMatch<Duplicate>(), 1);
  EXPECT_EQ(CountInstructionMatch<JumpIf>(), 1);
  EXPECT_EQ(CountInstructionMatch<Jump>(), 1);
  EXPECT_EQ(CountInstructionMatch<Call>(), 1);
  EXPECT_EQ(CountInstructionMatch<Return>(), 1);
  EXPECT_EQ(CountInstructionMatch<PushOne>(), 0);

#if defined(JASMIN_DEBUG)
  EXPECT_DEATH({ Set::InstructionFunction(Set::size()); },
               "Out-of-bounds op-code");
#endif  // defined(JASMIN_DEBUG)
}

TEST(ImmediateValueCount, Value) {
  EXPECT_EQ(internal::ImmediateValueCount<Return>(), 0);
  EXPECT_EQ(internal::ImmediateValueCount<Call>(), 0);
  EXPECT_EQ(internal::ImmediateValueCount<Jump>(), 1);
  EXPECT_EQ(internal::ImmediateValueCount<JumpIf>(), 1);

  struct NoImmediates : StackMachineInstruction<NoImmediates> {
    static int execute(int, int) { return 0; }
  };
  EXPECT_EQ(internal::ImmediateValueCount<NoImmediates>(), 0);

  struct EmptyImmediates : StackMachineInstruction<EmptyImmediates> {
    static void execute(ValueStack&) {}
  };
  EXPECT_EQ(internal::ImmediateValueCount<EmptyImmediates>(), 0);

  struct SomeImmediates : StackMachineInstruction<SomeImmediates> {
    static void execute(ValueStack&, int, bool) {}
  };
  EXPECT_EQ(internal::ImmediateValueCount<SomeImmediates>(), 2);
}

TEST(InstructionSet, FunctionStateStack) {
  struct Stateless : StackMachineInstruction<Stateless> {
    static int execute(int, int) { return 0; }
  };
  struct Stateful : StackMachineInstruction<Stateful> {
    using JasminFunctionState = int;
    static int execute(int, int) { return 0; }
  };
  using Set = MakeInstructionSet<Stateless, Stateful>;
  EXPECT_TRUE((std::is_same_v<internal::FunctionStateList<Set>,
                              internal::type_list<int>>));
  EXPECT_TRUE(
      (std::is_same_v<FunctionStateStack<Set>, std::stack<std::tuple<int>>>));
  EXPECT_TRUE(
      (std::is_same_v<FunctionStateStack<MakeInstructionSet<Stateless>>, void>));
  EXPECT_TRUE((std::is_same_v<FunctionStateStack<MakeInstructionSet<Stateful>>,
                              std::stack<std::tuple<int>>>));
}

}  // namespace
}  // namespace jasmin
