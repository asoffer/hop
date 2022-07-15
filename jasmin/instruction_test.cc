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

}  // namespace
}  // namespace jasmin
