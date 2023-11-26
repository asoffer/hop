#include "jasmin/instruction.h"

#include "jasmin/internal/instruction_traits.h"
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

struct NoOp : Instruction<NoOp> {
  static void execute(std::span<Value, 0>) {}
};
struct Duplicate : Instruction<Duplicate> {
  static Value execute(std::span<Value, 1> values) { return values[0]; }
};
struct PushOne : Instruction<PushOne> {
  static Value execute(std::span<Value, 0>) { return 1; }
};

using Set = MakeInstructionSet<NoOp, Duplicate>;

template <InstructionType I>
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

NTH_TEST("instruction/construction") {
  NTH_EXPECT(CountInstructionMatch<NoOp>() == 1u);
  NTH_EXPECT(CountInstructionMatch<Duplicate>() == 1u);
  NTH_EXPECT(CountInstructionMatch<JumpIf>() == 1u);
  NTH_EXPECT(CountInstructionMatch<Jump>() == 1u);
  NTH_EXPECT(CountInstructionMatch<Call>() == 1u);
  NTH_EXPECT(CountInstructionMatch<Return>() == 1u);
  NTH_EXPECT(CountInstructionMatch<PushOne>() == 0u);

  // TODO: Expect out-of-bounds op-code causes crash in hardened build.
}

NTH_TEST("instruction-set/state") {
  struct None : Instruction<None> {
    static int execute(std::span<Value, 0>, int) { return 0; }
  };
  struct F : Instruction<F> {
    using function_state = int;
    static int execute(function_state&, std::span<Value, 0>, int) { return 0; }
  };

  using Set = MakeInstructionSet<None, F>;
  NTH_EXPECT(internal::FunctionStateList<Set> == nth::type_sequence<int>);
  NTH_EXPECT(nth::type<internal::FunctionState<Set>> ==
             nth::type<std::tuple<int>>);
  NTH_EXPECT(nth::type<internal::FunctionState<MakeInstructionSet<None>>> ==
             nth::type<void>);
  NTH_EXPECT(nth::type<internal::FunctionState<MakeInstructionSet<F>>> ==
             nth::type<std::tuple<int>>);
}

NTH_TEST("instruction/opcode-metadata") {
  struct None : Instruction<None> {
    static void execute(std::span<Value, 0>, int) {}
  };
  struct F : Instruction<F> {
    using function_state = int;
    static void execute(function_state&, std::span<Value, 0>, int) {}
  };

  using Set = MakeInstructionSet<None, F>;
  NTH_EXPECT(Set::OpCodeMetadataFor<None>().op_code_value == 4u);
  NTH_EXPECT(Set::OpCodeMetadataFor<None>().immediate_value_count == 1u);
  NTH_EXPECT(Set::OpCodeMetadataFor<F>().op_code_value == 5u);
  NTH_EXPECT(Set::OpCodeMetadataFor<F>().immediate_value_count == 1u);

  NTH_EXPECT(Set::OpCodeMetadata(None::ExecuteImpl<Set>) ==
             Set::OpCodeMetadataFor<None>());
  NTH_EXPECT(Set::OpCodeMetadata(F::ExecuteImpl<Set>) ==
             Set::OpCodeMetadataFor<F>());
}

}  // namespace
}  // namespace jasmin
