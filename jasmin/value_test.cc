#include "jasmin/value.h"

#include <concepts>

#include "gtest/gtest.h"

namespace jasmin {
namespace {

TEST(Value, Construction) {
  EXPECT_TRUE((std::constructible_from<Value, char>));
  struct BarelyFits {
    char data[internal_value::ValueSize];
  };
  struct Large {
    char data[internal_value::ValueSize + 1];
  };
  EXPECT_TRUE((std::constructible_from<Value, BarelyFits>));
  EXPECT_FALSE((std::constructible_from<Value, Large>));

  struct alignas(2 * internal_value::ValueAlignment)
      OverlyStrictAlignmentRequirement {};
  EXPECT_FALSE(
      (std::constructible_from<Value, OverlyStrictAlignmentRequirement>));

  struct NotTriviallyCopyable {
    NotTriviallyCopyable(NotTriviallyCopyable const &) {}
  };
  EXPECT_FALSE((std::constructible_from<Value, NotTriviallyCopyable>));
}

TEST(Value, Access) {
  Value v(0);
  auto copy = v.as<Value>();
  EXPECT_EQ(std::memcmp(&copy, &v, sizeof(Value)), 0);
  EXPECT_EQ(v.as<int>(), 0);
#if defined(JASMIN_DEBUG)
  EXPECT_DEATH({ v.as<unsigned int>(); }, "Value type mismatch");
#endif  // defined(JASMIN_DEBUG)

  v    = 1;
  copy = v.as<Value>();
  EXPECT_EQ(std::memcmp(&copy, &v, sizeof(Value)), 0);
  EXPECT_EQ(v.as<int>(), 1);

  v    = true;
  copy = v.as<Value>();
  EXPECT_EQ(std::memcmp(&copy, &v, sizeof(Value)), 0);
  EXPECT_TRUE(v.as<bool>());
#if defined(JASMIN_DEBUG)
  EXPECT_DEATH({ v.as<int>(); }, "Value type mismatch");
#endif  // defined(JASMIN_DEBUG)
}

TEST(Value, Address) {
  Value v(17);
  void const * ptr = v.address();
  EXPECT_EQ(*reinterpret_cast<int const *>(ptr), 17);
}

}  // namespace
}  // namespace jasmin
