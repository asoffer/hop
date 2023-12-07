#ifndef JASMIN_SSA_OP_CODE_H
#define JASMIN_SSA_OP_CODE_H

#include <cstddef>
#include <string>

#include "absl/container/flat_hash_map.h"
#include "jasmin/core/instruction.h"
#include "nth/debug/debug.h"
#include "nth/meta/sequence.h"
#include "nth/meta/type.h"

namespace jasmin {

struct OpCodeMetadata {
  std::string name;
  size_t immediate_value_count;
  size_t parameter_count;
  size_t return_count;
  bool consumes_input;
};

template <InstructionType I>
OpCodeMetadata OpCodeMetadataFor() {
  return {
      .name                  = InstructionName<I>(),
      .immediate_value_count = ImmediateValueCount<I>(),
      .parameter_count       = ParameterCount<I>(),
      .return_count          = ReturnCount<I>(),
      .consumes_input        = ConsumesInput<I>(),
  };
}

namespace internal {

template <InstructionSetType S>
absl::flat_hash_map<exec_fn_type, OpCodeMetadata> MetadataMap =
    S::instructions.reduce([](auto... ts) {
      return absl::flat_hash_map<exec_fn_type, OpCodeMetadata>{
          {&nth::type_t<ts>::template ExecuteImpl<S>,
           OpCodeMetadataFor<nth::type_t<ts>>()}...};
    });

}  // namespace internal

template <InstructionSetType S>
OpCodeMetadata DecodeOpCode(Value v) {
  auto iter = internal::MetadataMap<S>.find(v.as<internal::exec_fn_type>());
  NTH_REQUIRE((v.when(internal::debug)),
              iter != internal::MetadataMap<S>.end());
  return iter->second;
}

}  // namespace jasmin

#if 0
template <InstructionType... Is>
struct MakeInstructionSet : InstructionSetBase {
  using self_type                    = MakeInstructionSet;
  static constexpr auto instructions = nth::type_sequence<Is...>;

  // Returns the number of instructions in the instruction set.
  static constexpr size_t size() { return sizeof...(Is); }

  // Returns the `internal::OpCodeMetadata` struct corresponding to the
  // op-code as specified in the byte-code.
  static internal::OpCodeMetadata OpCodeMetadata(Value v) {
    auto iter = Metadata.find(v.as<exec_fn_type>());
    NTH_REQUIRE((v.when(internal::harden)), iter != Metadata.end());
    return iter->second;
  }

  // Returns a `uint64_t` indicating the op-code for the given template
  // parameter instruction `I`.
  template <nth::any_of<Is...> I>
  static constexpr uint64_t OpCodeFor() {
    constexpr size_t value = OpCodeForImpl<I>();
    return value;
  }

  // Returns a `uint64_t` indicating the op-code for the given template
  // parameter instruction `I`.
  template <nth::any_of<Is...> I>
  static constexpr internal::OpCodeMetadata OpCodeMetadataFor() {
    return {.op_code_value         = OpCodeFor<I>(),
            .immediate_value_count = ImmediateValueCount<I>(),
            .parameter_count       = ParameterCount<I>()};
  }

  static auto InstructionFunction(uint64_t op_code) {
    NTH_REQUIRE(op_code < sizeof...(Is)).Log<"Out-of-bounds op-code.">();
    return table[op_code];
  }

 private:
  static constexpr exec_fn_type table[sizeof...(Is)] = {
      &Is::template ExecuteImpl<MakeInstructionSet>...};

  static absl::flat_hash_map<exec_fn_type, internal::OpCodeMetadata> const
      Metadata;

  template <nth::any_of<Is...> I>
  static constexpr uint64_t OpCodeForImpl() {
    // Because the fold-expression below unconditionally adds one to `i` on
    // its first evaluation, we start `i` at its maximum value and allow it to
    // wrap around.
    uint64_t i = std::numeric_limits<uint64_t>::max();
    static_cast<void>(((++i, std::is_same_v<I, Is>) or ...));
    return i;
  }
};

template <InstructionType... Is>
absl::flat_hash_map<exec_fn_type, internal::OpCodeMetadata> const
    MakeInstructionSet<Is...>::Metadata = [] {
      absl::flat_hash_map<exec_fn_type, internal::OpCodeMetadata> result;
      (result.emplace(&Is::template ExecuteImpl<self_type>,
                      OpCodeMetadataFor<Is>()),
       ...);
      return result;
    }();

/////////

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

#endif

#endif  // JASMIN_SSA_OP_CODE_H
