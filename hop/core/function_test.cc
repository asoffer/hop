#include "hop/core/function.h"

#include "nth/test/test.h"

namespace hop {
namespace {

struct PushImmediateBool : hop::Instruction<PushImmediateBool> {
  static constexpr void execute(Input<>, Output<bool> out, bool b) {
    out.set<0>(b);
  }
};

struct ImmediateDetermined : hop::Instruction<ImmediateDetermined> {
  static constexpr void execute(std::span<Value>, std::span<Value>, int) {}
};

using Instructions =
    hop::MakeInstructionSet<PushImmediateBool, ImmediateDetermined>;

NTH_TEST("function/append-incorrect-type") {
  bool converted = false;

  struct Converter {
    explicit Converter(bool* b) : converted_(b) {}
    operator bool() const {
      *converted_ = true;
      return true;
    }
    bool* converted_;
  };

  hop::Function<Instructions> func(0, 1);

  Converter c(&converted);
  NTH_EXPECT(not converted);
  // `c` Should be cast to `bool` in call to `append`.
  func.append<PushImmediateBool>(c);

  func.append<ImmediateDetermined>({.parameters = 0, .returns = 0}, 1);
  NTH_EXPECT(converted);
}

}  // namespace
}  // namespace hop
