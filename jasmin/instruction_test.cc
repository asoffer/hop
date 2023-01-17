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
struct Count : StackMachineInstruction<Count> {
  using JasminFunctionState = int;
  static void execute(ValueStack& v, JasminFunctionState& state) {
    v.push(state++);
  }
};

struct CountAcrossFunctions : StackMachineInstruction<CountAcrossFunctions> {
  using JasminExecutionState = int;
  static void execute(ValueStack& v, JasminExecutionState& state) {
    v.push(state++);
  }
};

struct CountBoth : StackMachineInstruction<CountBoth> {
  using JasminExecutionState = int;
  using JasminFunctionState = int;
  static void execute(ValueStack& v, JasminExecutionState& exec_state,
                      JasminFunctionState& fn_state) {
    v.push(exec_state++);
    v.push(fn_state++);
  }
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
  EXPECT_EQ(internal::ImmediateValueCount<Count>(), 0);
  EXPECT_EQ(internal::ImmediateValueCount<CountAcrossFunctions>(), 0);
  EXPECT_EQ(internal::ImmediateValueCount<CountBoth>(), 0);

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

TEST(InstructionSet, State) {
  struct None : StackMachineInstruction<None> {
    static int execute(int) { return 0; }
  };
  struct F : StackMachineInstruction<F> {
    using JasminFunctionState = int;
    static int execute(JasminFunctionState&, int) { return 0; }
  };
  struct E : StackMachineInstruction<E> {
    using JasminExecutionState = char;
    static int execute(JasminExecutionState&, int) { return 0; }
  };
  struct EF : StackMachineInstruction<EF> {
    using JasminExecutionState = char;
    using JasminFunctionState  = int;
    static int execute(JasminExecutionState&, JasminFunctionState&, int) {
      return 0;
    }
  };

  using Set = MakeInstructionSet<None, E, F, EF>;
  EXPECT_TRUE((std::is_same_v<internal::FunctionStateList<Set>,
                              internal::type_list<int>>));
  EXPECT_TRUE((std::is_same_v<internal::ExecutionStateList<Set>,
                              internal::type_list<char>>));
  EXPECT_TRUE(
      (std::is_same_v<FunctionStateStack<Set>, std::stack<std::tuple<int>>>));
  EXPECT_TRUE(
      (std::is_same_v<FunctionStateStack<MakeInstructionSet<None>>, void>));
  EXPECT_TRUE(
      (std::is_same_v<FunctionStateStack<MakeInstructionSet<E>>, void>));
  EXPECT_TRUE((std::is_same_v<FunctionStateStack<MakeInstructionSet<F>>,
                              std::stack<std::tuple<int>>>));
  EXPECT_TRUE((std::is_same_v<FunctionStateStack<MakeInstructionSet<EF>>,
                              std::stack<std::tuple<int>>>));
}

}  // namespace
}  // namespace jasmin
