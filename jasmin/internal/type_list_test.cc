#include "jasmin/internal/type_list.h"

#include <unordered_map>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace jasmin::internal {
namespace {

using ::testing::IsEmpty;
using ::testing::Pair;
using ::testing::UnorderedElementsAre;

TEST(TypeList, Concept) {
  EXPECT_TRUE((TypeList<type_list<>>));
  EXPECT_TRUE((TypeList<type_list<int>>));
  EXPECT_TRUE((TypeList<type_list<int, bool>>));
  EXPECT_FALSE(TypeList<void()>);
  EXPECT_FALSE(TypeList<int (*)()>);
  EXPECT_FALSE(TypeList<void(int*)>);
  EXPECT_FALSE(TypeList<int (*)(int*)>);
  EXPECT_FALSE(TypeList<void(int*, bool*)>);
  EXPECT_FALSE(TypeList<int (*)(int*, bool*)>);
  EXPECT_FALSE(TypeList<void (*)(int)>);
  EXPECT_FALSE(TypeList<int (*)(int*, bool)>);
}

template <typename... Ts>
using Voidify = void;

template <typename... Ts>
using Count = std::integral_constant<size_t, sizeof...(Ts)>;

TEST(TypeList, Apply) {
  EXPECT_TRUE((std::is_same_v<Apply<Voidify, type_list<>>, void>));
  EXPECT_TRUE((std::is_same_v<Apply<Voidify, type_list<int>>, void>));
  EXPECT_TRUE(
      (std::is_same_v<Apply<Voidify, type_list<int, bool, char>>, void>));

  EXPECT_TRUE((std::is_same_v<Apply<Count, type_list<>>,
                              std::integral_constant<size_t, 0>>));
  EXPECT_TRUE((std::is_same_v<Apply<Count, type_list<int>>,
                              std::integral_constant<size_t, 1>>));
  EXPECT_TRUE((std::is_same_v<Apply<Count, type_list<int, bool, char>>,
                              std::integral_constant<size_t, 3>>));
}

TEST(TypeList, ForEach) {
  std::map<int, int> sizes;
  ForEach<type_list<>>([&]<typename T>() { ++sizes[sizeof(T)]; });
  EXPECT_THAT(sizes, IsEmpty());

  sizes.clear();
  ForEach<type_list<int>>([&]<typename T>() { ++sizes[sizeof(T)]; });
  EXPECT_THAT(sizes, UnorderedElementsAre(Pair(sizeof(int), 1)));

  sizes.clear();
  ForEach<type_list<int8_t, int32_t, uint32_t>>(
      [&]<typename T>() { ++sizes[sizeof(T)]; });
  EXPECT_THAT(sizes, UnorderedElementsAre(Pair(1, 1), Pair(4, 2)));
}

TEST(ToNth, ToNth) {
  EXPECT_EQ(ToNth(type_list<>{}), nth::type_sequence<>);
  EXPECT_EQ(ToNth(type_list<int>{}), nth::type_sequence<int>);
  EXPECT_EQ(ToNth(type_list<int, bool>{}), (nth::type_sequence<int, bool>));
}

TEST(FromNth, FromNth) {
  EXPECT_TRUE((std::is_same_v<type_list<>, FromNth<nth::type_sequence<>>>));
  EXPECT_TRUE(
      (std::is_same_v<type_list<int>, FromNth<nth::type_sequence<int>>>));
  EXPECT_TRUE((std::is_same_v<type_list<int, bool>,
                             FromNth<nth::type_sequence<int, bool>>>));
}

}  // namespace
}  // namespace jasmin::internal
