#include "jasmin/internal/instruction_traits.h"

#include "jasmin/instruction.h"
#include "nth/test/test.h"

namespace jasmin {
namespace {

struct Count : Instruction<Count> {
  using function_state = int;
  static int execute(function_state& state, std::span<Value, 0>) {
    return state++;
  }
};

struct NoImmediates : Instruction<NoImmediates> {
  static int execute(std::span<Value, 2>) { return 0; }
};

struct NoImmediatesOrValues : Instruction<NoImmediatesOrValues> {
  static void execute(std::span<Value, 0>) {}
};

struct SomeImmediates : Instruction<SomeImmediates> {
  static void execute(std::span<Value, 0>, int, bool) {}
};

NTH_TEST("immediate-value-count") {
  NTH_EXPECT(ImmediateValueCount<Return>() == size_t{0});
  NTH_EXPECT(ImmediateValueCount<Call>() == size_t{0});
  NTH_EXPECT(ImmediateValueCount<Jump>() == size_t{1});
  NTH_EXPECT(ImmediateValueCount<JumpIf>() == size_t{1});
  NTH_EXPECT(ImmediateValueCount<Count>() == size_t{0});
  NTH_EXPECT(ImmediateValueCount<NoImmediates>() == size_t{0});
  NTH_EXPECT(ImmediateValueCount<NoImmediatesOrValues>() == size_t{0});
  NTH_EXPECT(ImmediateValueCount<SomeImmediates>() == size_t{2});
}

}  // namespace
}  // namespace jasmin

#if 0
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

  using Set = MakeInstructionSet<None, F>;
  EXPECT_EQ(internal::FunctionStateList<Set>, nth::type_sequence<int>);
  EXPECT_EQ(nth::type<internal::FunctionState<Set>>,
            nth::type<std::tuple<int>>);
  EXPECT_EQ(nth::type<internal::FunctionState<MakeInstructionSet<None>>>,
            nth::type<void>);
  EXPECT_EQ(nth::type<internal::FunctionState<MakeInstructionSet<F>>>,
            nth::type<std::tuple<int>>);
}

TEST(Instruction, OpCodeMetadata) {
  struct None : StackMachineInstruction<None> {
    static void execute(ValueStack&, int) {}
  };
  struct F : StackMachineInstruction<F> {
    using function_state = int;
    static void execute(ValueStack&, function_state&, int) {}
  };

  using Set = MakeInstructionSet<None, F>;
  EXPECT_EQ(Set::OpCodeMetadataFor<None>().op_code_value, 4);
  EXPECT_EQ(Set::OpCodeMetadataFor<None>().immediate_value_count, 1);
  EXPECT_EQ(Set::OpCodeMetadataFor<F>().op_code_value, 5);
  EXPECT_EQ(Set::OpCodeMetadataFor<F>().immediate_value_count, 1);

  EXPECT_EQ(Set::OpCodeMetadata(None::ExecuteImpl<Set>),
            Set::OpCodeMetadataFor<None>());
  EXPECT_EQ(Set::OpCodeMetadata(F::ExecuteImpl<Set>),
            Set::OpCodeMetadataFor<F>());
}

}  // namespace
}  // namespace jasmin
#endif
