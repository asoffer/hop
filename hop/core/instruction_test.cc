#include "hop/core/instruction.h"

#include "hop/core/internal/instruction_traits.h"
#include "nth/test/test.h"

namespace hop {
namespace {

struct Count : Instruction<Count> {
  using function_state = int;
  static void execute(function_state& state, Input<>, Output<int> out) {
    out.set<0>(state++);
  }
};

struct NoImmediates : Instruction<NoImmediates> {
  static void execute(Input<int, int>, Output<int> out) { out.set<0>(0); }
};

struct NoImmediatesOrValues : Instruction<NoImmediatesOrValues> {
  static void execute(Input<>, Output<>) {}
};

struct SomeImmediates : Instruction<SomeImmediates> {
  static void execute(Input<>, Output<>, int, bool) {}
};

struct ReturnsMultiple : Instruction<ReturnsMultiple> {
  static void consume(Input<>, Output<int, int> out) {
    out.set<0>(1);
    out.set<1>(2);
  }
};

NTH_TEST("immediate-value-count") {
  NTH_EXPECT(ImmediateValueCount<Return>() == size_t{0});
  NTH_EXPECT(ImmediateValueCount<Call>() == size_t{1});
  NTH_EXPECT(ImmediateValueCount<Jump>() == size_t{1});
  NTH_EXPECT(ImmediateValueCount<JumpIf>() == size_t{1});
  NTH_EXPECT(ImmediateValueCount<Count>() == size_t{0});
  NTH_EXPECT(ImmediateValueCount<NoImmediates>() == size_t{0});
  NTH_EXPECT(ImmediateValueCount<NoImmediatesOrValues>() == size_t{0});
  NTH_EXPECT(ImmediateValueCount<SomeImmediates>() == size_t{2});
}

NTH_TEST("consumes-input") {
  NTH_EXPECT(not ConsumesInput<Return>());
  NTH_EXPECT(ConsumesInput<Call>());
  NTH_EXPECT(not ConsumesInput<Jump>());
  NTH_EXPECT(ConsumesInput<JumpIf>());
  NTH_EXPECT(not ConsumesInput<Count>());
  NTH_EXPECT(not ConsumesInput<NoImmediates>());
  NTH_EXPECT(not ConsumesInput<NoImmediatesOrValues>());
  NTH_EXPECT(not ConsumesInput<SomeImmediates>());
}

NTH_TEST("return-count") {
  NTH_EXPECT(ReturnCount<Return>() == size_t{0});
  NTH_EXPECT(ReturnCount<Jump>() == size_t{0});
  NTH_EXPECT(ReturnCount<JumpIf>() == size_t{0});
  NTH_EXPECT(ReturnCount<Count>() == size_t{1});
  NTH_EXPECT(ReturnCount<NoImmediates>() == size_t{1});
  NTH_EXPECT(ReturnCount<NoImmediatesOrValues>() == size_t{0});
  NTH_EXPECT(ReturnCount<SomeImmediates>() == size_t{0});
  NTH_EXPECT(ReturnCount<ReturnsMultiple>() == size_t{2});
}

}  // namespace
}  // namespace hop
