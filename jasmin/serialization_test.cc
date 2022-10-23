#include "jasmin/serialization.h"

#include "gtest/gtest.h"
#include "jasmin/instructions/core.h"

namespace jasmin {
namespace {

using Instructions = MakeInstructionSet<Push, Drop>;

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
  f.append<Push>(4);
  f.append<Drop>(2);
  auto g = RoundTrip(f);

  ASSERT_EQ(g.raw_instructions().size(), 6);
  EXPECT_EQ(g.raw_instructions()[1].as<int>(), 3);
  EXPECT_EQ(g.raw_instructions()[3].as<int>(), 4);
  EXPECT_EQ(g.raw_instructions()[5].as<int>(), 2);

  std::array<char, ValueSize> f_buffer, g_buffer;

  Value::Store(f.raw_instructions()[0], f_buffer.data(), ValueSize);
  Value::Store(g.raw_instructions()[0], g_buffer.data(), ValueSize);
  EXPECT_EQ(f_buffer, g_buffer);

  Value::Store(f.raw_instructions()[2], f_buffer.data(), ValueSize);
  Value::Store(g.raw_instructions()[2], g_buffer.data(), ValueSize);
  EXPECT_EQ(f_buffer, g_buffer);

  Value::Store(f.raw_instructions()[4], f_buffer.data(), ValueSize);
  Value::Store(g.raw_instructions()[4], g_buffer.data(), ValueSize);
  EXPECT_EQ(f_buffer, g_buffer);
}

}  // namespace
}  // namespace jasmin
