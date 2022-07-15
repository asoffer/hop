#include "jasmin/instruction.h"

#include "gtest/gtest.h"

namespace jasmin {
namespace {

struct NoOp : StackMachineInstruction<NoOp> {
  static void execute() {}
};
struct Duplicate : StackMachineInstruction<Duplicate> {
  static void execute(ValueStack & v) {
    v.push(v.peek_value());
  }
};

using Set = MakeInstructionSet<NoOp, Duplicate>;

TEST(Instruction, Construction) {
  EXPECT_EQ(Set::InstructionFunction(0), &NoOp::ExecuteImpl<Set>);
  EXPECT_EQ(Set::InstructionFunction(1), &Duplicate::ExecuteImpl<Set>);

#if defined(JASMIN_DEBUG)
  EXPECT_DEATH({ Set::InstructionFunction(2); }, "Out-of-bounds op-code");
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
