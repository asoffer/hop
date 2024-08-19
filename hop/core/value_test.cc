#include "hop/core/value.h"

#include <concepts>

#include "nth/test/test.h"

namespace hop {
namespace {

NTH_TEST("value/construction") {
  NTH_EXPECT(std::constructible_from<Value, char>);
  struct BarelyFits {
    char data[8];
  };
  struct Large {
    char data[9];
  };
  NTH_EXPECT(std::constructible_from<Value, BarelyFits>);
  NTH_EXPECT(not std::constructible_from<Value, Large>);

  struct alignas(16) OverlyStrictAlignmentRequirement {};
  NTH_EXPECT(
      not(std::constructible_from<Value, OverlyStrictAlignmentRequirement>));

  struct NotTriviallyCopyable {
    NotTriviallyCopyable(NotTriviallyCopyable const &) {}
  };
  NTH_EXPECT(not std::constructible_from<Value, NotTriviallyCopyable>);
}

NTH_TEST("value/access") {
  Value v(0);
  auto copy = v.as<Value>();
  NTH_EXPECT(std::memcmp(&copy, &v, sizeof(Value)) == 0);
  NTH_EXPECT(v.as<int>() == 0);

  v    = 1;
  copy = v.as<Value>();
  NTH_EXPECT(std::memcmp(&copy, &v, sizeof(Value)) == 0);
  NTH_EXPECT(v.as<int>() == 1);

  v    = true;
  copy = v.as<Value>();
  NTH_EXPECT(std::memcmp(&copy, &v, sizeof(Value)) == 0);
  NTH_EXPECT(v.as<bool>());
}

NTH_TEST("value/load") {
  int n   = 17;
  Value v = Value::Load(&n, sizeof(int));
  NTH_EXPECT(v.as<int>() == 17);
}

NTH_TEST("value/store") {
  int n   = 0;
  Value v = 17;
  Value::Store(v, &n, sizeof(int));
  NTH_EXPECT(n == 17);
}

NTH_TEST("value/raw") {
  Value v1 = 3.14159;
  Value v2 = 0;
  v2.set_raw_value(v1.raw_value());
  NTH_EXPECT(v1.as<double>() == v2.as<double>());
}

}  // namespace
}  // namespace hop
