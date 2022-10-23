#include "jasmin/serialization.h"

#include "gtest/gtest.h"
#include "jasmin/instructions/core.h"
#include "jasmin/instructions/stack.h"

namespace jasmin {
namespace {

using Instructions = MakeInstructionSet<Push, Drop, StackAllocate>;

Function<Instructions> RoundTrip(Function<Instructions> const& f) {
  Function<Instructions> result(f.parameter_count(), f.return_count());
  std::string s;
  Serialize(f, s);
  Deserialize(s, result);
  return result;
}

TEST(Serialization, Test) {
  Function<Instructions> f(0, 0);
  f.append<Push>(3);
  f.append<StackAllocate>(4);
  f.append<Drop>(2);
  auto g = RoundTrip(f);

  ASSERT_EQ(f.raw_instructions().size(), g.raw_instructions().size());
  std::array<char, ValueSize> f_buffer, g_buffer;
  for (size_t i = 0; i < f.raw_instructions().size(); ++i) {
    Value::Store(f.raw_instructions()[i], f_buffer.data(), ValueSize);
    Value::Store(g.raw_instructions()[i], g_buffer.data(), ValueSize);
    EXPECT_EQ(f_buffer, g_buffer);
  }
}

}  // namespace
}  // namespace jasmin
