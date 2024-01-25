#include "jasmin/core/metadata.h"

#include <type_traits>

#include "nth/test/test.h"

namespace jasmin {
namespace {

template <typename>
struct InputType;

template <int... Ns>
struct InputType<std::index_sequence<Ns...>> {
  template <int N>
  using JustValue = Value;
  using type      = Input<JustValue<Ns>...>;
};

template <int N>
struct Inst : jasmin::Instruction<Inst<N>> {
  static constexpr void execute(
      typename InputType<std::make_index_sequence<N>>::type, Output<bool> out,
      bool b) {
    out.set<0>(b);
  }
};

struct ImmediateDetermined : jasmin::Instruction<ImmediateDetermined> {
  static constexpr void execute(std::span<Value>, std::span<Value>, int, bool,
                                char) {}
};

using Instructions =
    jasmin::MakeInstructionSet<Inst<0>, Inst<4>, ImmediateDetermined>;

template <InstructionType I, InstructionSetType Set>
Value InstructionFunction = Value(&I::template ExecuteImpl<Set>);

NTH_TEST("metadata") {
  auto const& m = Metadata<Instructions>();
  NTH_EXPECT(m.size() == Instructions::instructions.size());

  NTH_EXPECT(m.function(0).raw_value() ==
             InstructionFunction<Call, Instructions>.raw_value());

  NTH_EXPECT(m.function(1).raw_value() ==
             InstructionFunction<Jump, Instructions>.raw_value());
  NTH_EXPECT(m.metadata(1).immediate_value_count == 1);
  NTH_EXPECT(m.metadata(1).parameter_count == 0);

  NTH_EXPECT(m.function(2).raw_value() ==
             InstructionFunction<JumpIf, Instructions>.raw_value());
  NTH_EXPECT(m.metadata(2).immediate_value_count == 1);
  NTH_EXPECT(m.metadata(2).parameter_count == 1);

  NTH_EXPECT(m.function(3).raw_value() ==
             InstructionFunction<Return, Instructions>.raw_value());
  NTH_EXPECT(m.metadata(3).immediate_value_count == 0);
  NTH_EXPECT(m.metadata(3).parameter_count == 0);

  NTH_EXPECT(m.function(4).raw_value() ==
             InstructionFunction<Inst<0>, Instructions>.raw_value());
  NTH_EXPECT(m.metadata(4).immediate_value_count == 1);
  NTH_EXPECT(m.metadata(4).parameter_count == 0);

  NTH_EXPECT(m.function(5).raw_value() ==
             InstructionFunction<Inst<4>, Instructions>.raw_value());
  NTH_EXPECT(m.metadata(5).immediate_value_count == 1);
  NTH_EXPECT(m.metadata(5).parameter_count == 4);

  NTH_EXPECT(
      m.function(6).raw_value() ==
      InstructionFunction<ImmediateDetermined, Instructions>.raw_value());
  // The immediate value count is 4. Three values are passed to `execute`, and a
  // fourth implicit value dictates the size of the input and output spans.
  NTH_EXPECT(m.metadata(6).immediate_value_count == 4);
  NTH_EXPECT(m.metadata(6).parameter_count ==
             std::numeric_limits<size_t>::max());
}

}  // namespace
}  // namespace jasmin
