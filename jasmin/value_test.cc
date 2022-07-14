#include "jasmin/value.h"

#include <concepts>

#include "gtest/gtest.h"

namespace jasmin {
namespace {

TEST(Value, Construction) {
  EXPECT_TRUE((std::constructible_from<Value, char>));
  struct BarelyFits {
    constexpr bool operator==(BarelyFits const&) = default;
    char data[internal_value::ValueSize];
  };
  struct Large {
    constexpr bool operator==(Large const&) = default;
    char data[internal_value::ValueSize + 1];
  };
  EXPECT_TRUE((std::constructible_from<Value, BarelyFits>));
  EXPECT_FALSE((std::constructible_from<Value, Large>));

  struct alignas(2 * internal_value::ValueAlignment) OverlyStrictAlignmentRequirement {
    constexpr bool operator==(OverlyStrictAlignmentRequirement const&) = default;
  };
  EXPECT_FALSE((std::constructible_from<Value, OverlyStrictAlignmentRequirement>));

  struct NotTriviallyCopyable {
    constexpr bool operator==(NotTriviallyCopyable const&) = default;
    NotTriviallyCopyable(NotTriviallyCopyable const &) {}
  };
  EXPECT_FALSE((std::constructible_from<Value, NotTriviallyCopyable>));
}

TEST(Value, Access) {
  Value v(0);
  EXPECT_EQ(v.as<int>(), 0);
#if defined(JASMIN_DEBUG)
  EXPECT_DEATH({ v.as<unsigned int>(); }, "Value type mismatch");
#endif  // defined(JASMIN_DEBUG)
  v = 1;
  EXPECT_EQ(v.as<int>(), 1);

  v = true;
  EXPECT_TRUE(v.as<bool>());
#if defined(JASMIN_DEBUG)
  EXPECT_DEATH({ v.as<int>(); }, "Value type mismatch");
#endif  // defined(JASMIN_DEBUG)
}

}  // namespace
}  // namespace jasmin
