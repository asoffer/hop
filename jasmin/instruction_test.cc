#include "jasmin/instruction.h"

#include "gtest/gtest.h"
#include "nth/debug/log/log.h"
#include "nth/debug/log/stderr_log_sink.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace jasmin {
namespace {

bool init = [] {
  nth::RegisterLogSink(nth::stderr_log_sink);
  return false;
}();

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

  if constexpr (internal::harden) {
    EXPECT_DEATH({ Set::InstructionFunction(Set::size()); },
                 "Out-of-bounds op-code");
  }
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
