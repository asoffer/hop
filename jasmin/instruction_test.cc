#include "jasmin/instruction.h"

#include "gtest/gtest.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

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
  using function_state = int;
  static void execute(ValueStack& v, function_state& state) { v.push(state++); }
};

struct CountAcrossFunctions : StackMachineInstruction<CountAcrossFunctions> {
  using execution_state = int;
  static void execute(ValueStack& v, execution_state& state) {
    v.push(state++);
  }
};

struct CountBoth : StackMachineInstruction<CountBoth> {
  using execution_state = int;
  using function_state  = int;
  static void execute(ValueStack& v, execution_state& exec_state,
                      function_state& fn_state) {
    v.push(exec_state++);
    v.push(fn_state++);
  }
};

using Set = MakeInstructionSet<NoOp, Duplicate>;

template <Instruction I>
size_t CountInstructionMatch() {
  size_t count = 0;
  for (size_t i = 0; i < Set::size(); ++i) {
    if (Set::InstructionFunction(i) ==
        &I::template ExecuteImpl<typename Set::self_type>) {
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
    using function_state = int;
    static int execute(function_state&, int) { return 0; }
  };
  struct E : StackMachineInstruction<E> {
    using execution_state = char;
    static int execute(execution_state&, int) { return 0; }
  };
  struct EF : StackMachineInstruction<EF> {
    using execution_state = char;
    using function_state  = int;
    static int execute(execution_state&, function_state&, int) { return 0; }
  };

  using Set = MakeInstructionSet<None, E, F, EF>;
  EXPECT_EQ(internal::FunctionStateList<Set>, nth::type_sequence<int>);
  EXPECT_EQ(internal::ExecutionStateList<Set>, nth::type_sequence<char>);
  EXPECT_EQ(nth::type<internal::FunctionStateStack<Set>>,
            nth::type<std::stack<std::tuple<int>>>);
  EXPECT_EQ(nth::type<internal::FunctionStateStack<MakeInstructionSet<None>>>,
            nth::type<void>);
  EXPECT_EQ(nth::type<internal::FunctionStateStack<MakeInstructionSet<E>>>,
            nth::type<void>);
  EXPECT_EQ(nth::type<internal::FunctionStateStack<MakeInstructionSet<F>>>,
            nth::type<std::stack<std::tuple<int>>>);
  EXPECT_EQ(nth::type<internal::FunctionStateStack<MakeInstructionSet<EF>>>,
            nth::type<std::stack<std::tuple<int>>>);
}

}  // namespace
}  // namespace jasmin
