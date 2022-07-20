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
  EXPECT_EQ(internal_instruction::ImmediateValueCount<Return>(), 0);
  EXPECT_EQ(internal_instruction::ImmediateValueCount<Call>(), 0);
  EXPECT_EQ(internal_instruction::ImmediateValueCount<Jump>(), 1);
  EXPECT_EQ(internal_instruction::ImmediateValueCount<JumpIf>(), 1);

  struct NoImmediates : StackMachineInstruction<NoImmediates> {
    static int execute(int, int) { return 0; }
  };
  EXPECT_EQ(internal_instruction::ImmediateValueCount<NoImmediates>(), 0);

  struct EmptyImmediates : StackMachineInstruction<EmptyImmediates> {
    static void execute(ValueStack&) {}
  };
  EXPECT_EQ(internal_instruction::ImmediateValueCount<EmptyImmediates>(), 0);

  struct SomeImmediates : StackMachineInstruction<SomeImmediates> {
    static void execute(ValueStack&, int, bool) {}
  };
  EXPECT_EQ(internal_instruction::ImmediateValueCount<SomeImmediates>(), 2);
}
}  // namespace
}  // namespace jasmin
