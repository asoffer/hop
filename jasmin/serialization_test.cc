#include "jasmin/serialization.h"

#include "gtest/gtest.h"
#include "jasmin/instructions/core.h"
#include "jasmin/instructions/stack.h"

namespace jasmin {
namespace {

using Instructions = MakeInstructionSet<Push, Drop, StackAllocate>;

template <InstructionSet Set>
Function<Set> RoundTrip(Function<Set> const& f) {
  Function<Set> result(f.parameter_count(), f.return_count());
  std::string s;
  Serialize(f, s);
  EXPECT_TRUE(Deserialize(s, result));
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

struct DefaultSerialization : StackMachineInstruction<DefaultSerialization> {
  static constexpr void execute(ValueStack&, int64_t) {}
};

struct TestState {
  int stateless_count = 0;
  int stateful_count  = 0;
} state;

struct StatelessSerialization
   : StackMachineInstruction<StatelessSerialization> {
  using serialization_state = void;

  static void serialize(Serializer& serializer,
                        std::span<Value const, 1> values) {
    serializer(static_cast<int32_t>(values[0].as<int32_t>() * 2));
    ++state.stateless_count;
  }

  static bool deserialize(Deserializer& deserializer, std::span<Value, 1> values) {
    int32_t value;
    if (not deserializer(value)) { return false; }
    values[0] = value / 2;
    --state.stateless_count;
    return true;
  }

  static constexpr void execute(ValueStack&, int) {}
};

struct StatefulSerialization : StackMachineInstruction<StatefulSerialization> {
  static constexpr void execute(ValueStack&, int32_t) {}

  using serialization_state = std::vector<int>;

  static void serialize(Serializer& serializer, std::span<Value const> values,
                        std::vector<int>& v) {
    v.push_back(values[0].as<int32_t>());
    serializer(v.size());
    ++state.stateful_count;
  }
  static bool deserialize(Deserializer& deserializer, std::span<Value> values,
                          std::span<int const> v) {
    size_t n;
    if (not deserializer(n)) { return false; }
    values[0] = static_cast<int32_t>(v[n - 1]);
    --state.stateful_count;
    return true;
  }
};

struct SerializationState {
  template <typename T>
  T& get() {
    static_assert(std::is_same_v<T, std::vector<int>>);
    return v_;
  }

  std::vector<int> v_;
};

using CustomSerializationInstructions =
    MakeInstructionSet<DefaultSerialization, StatelessSerialization,
                       StatefulSerialization>;

TEST(SerializationStateType, SerializationStateType) {
  EXPECT_TRUE(
      (std::is_same_v<SerializationStateFor<DefaultSerialization>, void>));
  EXPECT_FALSE(HasCustomSerializer<DefaultSerialization>);
  EXPECT_TRUE(
      (std::is_same_v<SerializationStateFor<StatelessSerialization>, void>));
  EXPECT_TRUE(HasCustomSerializer<StatelessSerialization>);
  EXPECT_TRUE((std::is_same_v<SerializationStateFor<StatefulSerialization>,
                              std::vector<int>>));
  EXPECT_TRUE(HasCustomSerializer<StatefulSerialization>);

  Function<CustomSerializationInstructions> f(0, 0);
  f.append<DefaultSerialization>(3);
  f.append<StatelessSerialization>(4);
  f.append<StatefulSerialization>(10);
  f.append<StatefulSerialization>(20);
  f.append<StatefulSerialization>(10);

  state = {};

  Function<CustomSerializationInstructions> result(f.parameter_count(),
                                                   f.return_count());
  std::string s;
  SerializationState serialization_state;
  Serialize(f, s, serialization_state);

  EXPECT_EQ(state.stateless_count, 1);
  EXPECT_EQ(state.stateful_count, 3);

  EXPECT_EQ(s,
            std::string_view("\x04\x00\x00\x00\x00\x00\x00\x00"  // Op-code 4
                             "\x10\x00\x00\x00"  // 16 bytes encoded for
                             "\x01\x00\x00\x00"  // one immediate value
                             "\x03\x00\x00\x00\x00\x00\x00\x00"  // Value is 3

                             "\x05\x00\x00\x00\x00\x00\x00\x00"  // Op-code 5
                             "\x0c\x00\x00\x00"  // 12 bytes encoded for
                             "\x01\x00\x00\x00"  // one immediate value
                             "\x08\x00\x00\x00"  // Value is 4*2

                             "\x06\x00\x00\x00\x00\x00\x00\x00"  // Op-code 6
                             "\x10\x00\x00\x00"  // 16 bytes encoded for
                             "\x01\x00\x00\x00"  // one immediate value
                             "\x01\x00\x00\x00\x00\x00\x00\x00"  // Value is 1

                             "\x06\x00\x00\x00\x00\x00\x00\x00"  // Op-code 6
                             "\x10\x00\x00\x00"  // 16 bytes encoded for
                             "\x01\x00\x00\x00"  // one immediate value
                             "\x02\x00\x00\x00\x00\x00\x00\x00"  // Value is 2

                             "\x06\x00\x00\x00\x00\x00\x00\x00"  // Op-code 6
                             "\x10\x00\x00\x00"  // 16 bytes encoded for
                             "\x01\x00\x00\x00"  // one immediate value
                             "\x03\x00\x00\x00\x00\x00\x00\x00",  // Value is 3
                             116));
}

}  // namespace
}  // namespace jasmin
