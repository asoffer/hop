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

TEST(TypeList, Concatenate) {
  EXPECT_TRUE(
      (std::is_same_v<Concatenate<type_list<>, type_list<>>, type_list<>>));
  EXPECT_TRUE((std::is_same_v<Concatenate<type_list<int>, type_list<>>,
                              type_list<int>>));
  EXPECT_TRUE((std::is_same_v<Concatenate<type_list<>, type_list<int>>,
                              type_list<int>>));
  EXPECT_TRUE((std::is_same_v<Concatenate<type_list<int>, type_list<int>>,
                              type_list<int, int>>));
  EXPECT_TRUE((std::is_same_v<Concatenate<type_list<bool>, type_list<int>>,
                              type_list<bool, int>>));
  EXPECT_TRUE((std::is_same_v<Concatenate<type_list<int>, type_list<bool>>,
                              type_list<int, bool>>));
  EXPECT_TRUE(
      (std::is_same_v<Concatenate<type_list<int, bool>, type_list<char, void>>,
                      type_list<int, bool, char, void>>));
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

TEST(TypeList, Transform) {
  EXPECT_TRUE((
      std::is_same_v<Transform<std::add_pointer_t, type_list<>>, type_list<>>));
  EXPECT_TRUE((std::is_same_v<Transform<std::add_pointer_t, type_list<int*>>,
                              type_list<int**>>));
  EXPECT_TRUE(
      (std::is_same_v<Transform<std::add_pointer_t, type_list<bool, int*>>,
                      type_list<bool*, int**>>));
}

TEST(TypeList, Filter) {
  EXPECT_TRUE(
      (std::is_same_v<Filter<std::is_pointer, type_list<>>, type_list<>>));
  EXPECT_TRUE(
      (std::is_same_v<Filter<std::is_pointer, type_list<int>>, type_list<>>));
  EXPECT_TRUE((std::is_same_v<Filter<std::is_pointer, type_list<int*>>,
                              type_list<int*>>));
  EXPECT_TRUE((std::is_same_v<Filter<std::is_pointer, type_list<bool*, int>>,
                              type_list<bool*>>));
  EXPECT_TRUE((std::is_same_v<Filter<std::is_pointer, type_list<bool, int*>>,
                              type_list<int*>>));
  EXPECT_TRUE((std::is_same_v<Filter<std::is_pointer, type_list<bool, int>>,
                              type_list<>>));
  EXPECT_TRUE((std::is_same_v<Filter<std::is_pointer, type_list<bool*, int*>>,
                              type_list<bool*, int*>>));
}

TEST(TypeList, Unique) {
  EXPECT_TRUE((std::is_same_v<Unique<type_list<>>, type_list<>>));
  EXPECT_TRUE((std::is_same_v<Unique<type_list<int>>, type_list<int>>));
  EXPECT_TRUE(
      (std::is_same_v<Unique<type_list<bool, int>>, type_list<int, bool>>));
  EXPECT_TRUE((std::is_same_v<Unique<type_list<bool, int, bool>>,
                              type_list<int, bool>>));
  EXPECT_TRUE((std::is_same_v<Unique<type_list<bool, int, bool, bool, int>>,
                              type_list<int, bool>>));
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

}  // namespace
}  // namespace jasmin::internal
