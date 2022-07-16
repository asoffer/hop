#include "jasmin/internal/type_traits.h"

#include <tuple>
#include <type_traits>

#include "gtest/gtest.h"

namespace jasmin::internal_type_traits {
namespace {

TEST(ExtractSignature, ReturnType) {
  using A = ExtractSignature<void (*)()>;
  EXPECT_TRUE((std::is_same_v<void, typename A::return_type>));

  using B = ExtractSignature<void (*)(int, bool, char)>;
  EXPECT_TRUE((std::is_same_v<void, typename B::return_type>));

  using C = ExtractSignature<int (*)()>;
  EXPECT_TRUE((std::is_same_v<int, typename C::return_type>));

  using D = ExtractSignature<int (*)(int, bool, char)>;
  EXPECT_TRUE((std::is_same_v<int, typename D::return_type>));
}

TEST(ExtractSignature, Arguments) {
  using A = ExtractSignature<void (*)()>;
  int a   = 0;
  A::invoke_with_argument_types([&]<typename... Ts> {
    EXPECT_TRUE((std::is_same_v<std::tuple<Ts...>, std::tuple<>>));
    ++a;
  });
  ASSERT_EQ(a, 1);

  using B = ExtractSignature<void (*)(int, bool, char)>;
  int b   = 0;
  B::invoke_with_argument_types([&]<typename... Ts> {
    EXPECT_TRUE(
        (std::is_same_v<std::tuple<Ts...>, std::tuple<int, bool, char>>));
    ++b;
  });
  ASSERT_EQ(b, 1);

  using C = ExtractSignature<void (*)()>;
  int c   = 0;
  C::invoke_with_argument_types([&]<typename... Ts> {
    EXPECT_TRUE((std::is_same_v<std::tuple<Ts...>, std::tuple<>>));
    ++c;
  });
  ASSERT_EQ(c, 1);

  using D = ExtractSignature<void (*)(int, bool, char)>;
  int d   = 0;
  D::invoke_with_argument_types([&]<typename... Ts> {
    EXPECT_TRUE(
        (std::is_same_v<std::tuple<Ts...>, std::tuple<int, bool, char>>));
    ++d;
  });
  ASSERT_EQ(d, 1);
}

}  // namespace
}  // namespace jasmin::internal_type_traits
