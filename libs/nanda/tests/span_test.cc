
#include <gtest/gtest.h>

#include <vector>

#include "nanda/span.hh"

using namespace nanda;

TEST(SpanTest, DefaultConstructor)
{
  span<int> dynamic_span;
  EXPECT_EQ(nullptr, dynamic_span.data());
  EXPECT_EQ(0u, dynamic_span.size());
  constexpr span<int, 0> static_span;
  static_assert(nullptr == static_span.data(), "");
  static_assert(0u == static_span.size(), "");
}

TEST(SpanTest, ConstructFromDataAndSize)
{
  constexpr int* kNull = nullptr;
  constexpr span<int> empty_span(kNull, 0u);
  EXPECT_TRUE(empty_span.empty());
  EXPECT_EQ(nullptr, empty_span.data());
  std::vector<int> vector = { 1, 1, 2, 3, 5, 8 };
  span<int> dynamic_span(vector.data(), vector.size());
  EXPECT_EQ(vector.data(), dynamic_span.data());
  EXPECT_EQ(vector.size(), dynamic_span.size());
  for (size_t i = 0; i < dynamic_span.size(); ++i)
    EXPECT_EQ(vector[i], dynamic_span[i]);
  span<int, 6> static_span(vector.data(), vector.size());
  EXPECT_EQ(vector.data(), static_span.data());
  EXPECT_EQ(vector.size(), static_span.size());
  for (size_t i = 0; i < static_span.size(); ++i)
    EXPECT_EQ(vector[i], static_span[i]);
}
TEST(SpanTest, ConstructFromIterAndSize)
{
  constexpr int* kNull = nullptr;
  constexpr span<int> empty_span(kNull, 0u);
  EXPECT_TRUE(empty_span.empty());
  EXPECT_EQ(nullptr, empty_span.data());
  std::vector<int> vector = { 1, 1, 2, 3, 5, 8 };
  span<int> dynamic_span(vector.begin(), vector.size());
  EXPECT_EQ(vector.data(), dynamic_span.data());
  EXPECT_EQ(vector.size(), dynamic_span.size());
  for (size_t i = 0; i < dynamic_span.size(); ++i)
    EXPECT_EQ(vector[i], dynamic_span[i]);
  span<int, 6> static_span(vector.begin(), vector.size());
  EXPECT_EQ(vector.data(), static_span.data());
  EXPECT_EQ(vector.size(), static_span.size());
  for (size_t i = 0; i < static_span.size(); ++i)
    EXPECT_EQ(vector[i], static_span[i]);
}

TEST(SpanTest, AllowedConversionsFromStdArray)
{
  // In the following assertions we use std::is_convertible_v<From, To>, which
  // for non-void types is equivalent to checking whether the following
  // expression is well-formed:
  //
  // T obj = std::declval<From>();
  //
  // In particular we are checking whether From is implicitly convertible to To,
  // which also implies that To is explicitly constructible from From.
  static_assert(
    std::is_convertible<std::array<int, 3>&, nanda::span<int>>::value,
    "Error: l-value reference to std::array<int> should be convertible to "
    "nanda::span<int> with dynamic extent.");
  static_assert(
    std::is_convertible<std::array<int, 3>&, nanda::span<int, 3>>::value,
    "Error: l-value reference to std::array<int> should be convertible to "
    "nanda::span<int> with the same static extent.");
  static_assert(
    std::is_convertible<std::array<int, 3>&, nanda::span<const int>>::value,
    "Error: l-value reference to std::array<int> should be convertible to "
    "nanda::span<const int> with dynamic extent.");
  static_assert(
    std::is_convertible<std::array<int, 3>&, nanda::span<const int, 3>>::value,
    "Error: l-value reference to std::array<int> should be convertible to "
    "nanda::span<const int> with the same static extent.");
  static_assert(std::is_convertible<const std::array<int, 3>&,
                                    nanda::span<const int>>::value,
                "Error: const l-value reference to std::array<int> should be "
                "convertible to nanda::span<const int> with dynamic extent.");
  static_assert(
    std::is_convertible<const std::array<int, 3>&,
                        nanda::span<const int, 3>>::value,
    "Error: const l-value reference to std::array<int> should be convertible "
    "to nanda::span<const int> with the same static extent.");
  static_assert(std::is_convertible<std::array<const int, 3>&,
                                    nanda::span<const int>>::value,
                "Error: l-value reference to std::array<const int> should be "
                "convertible to nanda::span<const int> with dynamic extent.");
  static_assert(
    std::is_convertible<std::array<const int, 3>&,
                        nanda::span<const int, 3>>::value,
    "Error: l-value reference to std::array<const int> should be convertible "
    "to nanda::span<const int> with the same static extent.");
  // static_assert(
  //   std::is_convertible<const std::array<const int, 3>&,
  //                       nanda::span<const int>>::value,
  //   "Error: const l-value reference to std::array<const int> should be "
  //   "convertible to nanda::span<const int> with dynamic extent.");
  // static_assert(
  //   std::is_convertible<const std::array<const int, 3>&,
  //                       nanda::span<const int, 3>>::value,
  //   "Error: const l-value reference to std::array<const int> should be "
  //   "convertible to nanda::span<const int> with the same static extent.");
}
TEST(SpanTest, DisallowedConstructionsFromStdArray)
{
  // In the following assertions we use !std::is_constructible_v<T, Args>, which
  // is equivalent to checking whether the following expression is malformed:
  //
  // T obj(std::declval<Args>()...);
  //
  // In particular we are checking that T is not explicitly constructible from
  // Args, which also implies that T is not implicitly constructible from Args
  // as well.
  // static_assert(
  //   !std::is_constructible<nanda::span<int>, const std::array<int,
  //   3>&>::value, "Error: nanda::span<int> with dynamic extent should not be
  //   constructible " "from const l-value reference to std::array<int>");
  static_assert(
    !std::is_constructible<nanda::span<int>, std::array<const int, 3>&>::value,
    "Error: nanda::span<int> with dynamic extent should not be constructible "
    "from l-value reference to std::array<const int>");
  static_assert(
    !std::is_constructible<nanda::span<int>,
                           const std::array<const int, 3>&>::value,
    "Error: nanda::span<int> with dynamic extent should not be constructible "
    "const from l-value reference to std::array<const int>");
  static_assert(
    !std::is_constructible<nanda::span<int, 2>, std::array<int, 3>&>::value,
    "Error: nanda::span<int> with static extent should not be constructible "
    "from l-value reference to std::array<int> with different extent");
  static_assert(
    !std::is_constructible<nanda::span<int, 4>, std::array<int, 3>&>::value,
    "Error: nanda::span<int> with dynamic extent should not be constructible "
    "from l-value reference to std::array<int> with different extent");
  static_assert(
    !std::is_constructible<nanda::span<int>, std::array<bool, 3>&>::value,
    "Error: nanda::span<int> with dynamic extent should not be constructible "
    "from l-value reference to std::array<bool>");
}
TEST(SpanTest, ConstructFromConstexprArray)
{
  static constexpr int kArray[] = { 5, 4, 3, 2, 1 };
  constexpr span<const int> dynamic_span(kArray);
  static_assert(kArray == dynamic_span.data(), "");
  static_assert(std::size(kArray) == dynamic_span.size(), "");
  static_assert(kArray[0] == dynamic_span[0], "");
  static_assert(kArray[1] == dynamic_span[1], "");
  static_assert(kArray[2] == dynamic_span[2], "");
  static_assert(kArray[3] == dynamic_span[3], "");
  static_assert(kArray[4] == dynamic_span[4], "");
  constexpr span<const int, std::size(kArray)> static_span(kArray);
  static_assert(kArray == static_span.data(), "");
  static_assert(std::size(kArray) == static_span.size(), "");
  static_assert(kArray[0] == static_span[0], "");
  static_assert(kArray[1] == static_span[1], "");
  static_assert(kArray[2] == static_span[2], "");
  static_assert(kArray[3] == static_span[3], "");
  static_assert(kArray[4] == static_span[4], "");
}

TEST(SpanTest, ConstructFromArray)
{
  int array[] = { 5, 4, 3, 2, 1 };
  span<const int> const_span(array);
  EXPECT_EQ(array, const_span.data());
  EXPECT_EQ(std::size(array), const_span.size());
  for (size_t i = 0; i < const_span.size(); ++i)
    EXPECT_EQ(array[i], const_span[i]);
  span<int> dynamic_span(array);
  EXPECT_EQ(array, dynamic_span.data());
  EXPECT_EQ(std::size(array), dynamic_span.size());
  for (size_t i = 0; i < dynamic_span.size(); ++i)
    EXPECT_EQ(array[i], dynamic_span[i]);
  span<int, std::size(array)> static_span(array);
  EXPECT_EQ(array, static_span.data());
  EXPECT_EQ(std::size(array), static_span.size());
  for (size_t i = 0; i < static_span.size(); ++i)
    EXPECT_EQ(array[i], static_span[i]);
}

TEST(SpanTest, ConstructFromStdArray)
{
  // Note: Constructing a constexpr span from a constexpr std::array does not
  // work prior to C++17 due to non-constexpr std::array::data.
  std::array<int, 5> array = { { 5, 4, 3, 2, 1 } };
  span<const int> const_span(array);
  EXPECT_EQ(array.data(), const_span.data());
  EXPECT_EQ(array.size(), const_span.size());
  for (size_t i = 0; i < const_span.size(); ++i)
    EXPECT_EQ(array[i], const_span[i]);
  span<int> dynamic_span(array);
  EXPECT_EQ(array.data(), dynamic_span.data());
  EXPECT_EQ(array.size(), dynamic_span.size());
  for (size_t i = 0; i < dynamic_span.size(); ++i)
    EXPECT_EQ(array[i], dynamic_span[i]);
  span<int, std::size(array)> static_span(array);
  EXPECT_EQ(array.data(), static_span.data());
  EXPECT_EQ(array.size(), static_span.size());
  for (size_t i = 0; i < static_span.size(); ++i)
    EXPECT_EQ(array[i], static_span[i]);
}
TEST(SpanTest, ConstructFromInitializerList)
{
  std::initializer_list<int> il = { 1, 1, 2, 3, 5, 8 };
  span<const int> const_span(il);
  EXPECT_EQ(il.begin(), const_span.data());
  EXPECT_EQ(il.size(), const_span.size());
  for (size_t i = 0; i < const_span.size(); ++i)
    EXPECT_EQ(il.begin()[i], const_span[i]);
  span<const int, 6> static_span(il.begin(), il.end());
  EXPECT_EQ(il.begin(), static_span.data());
  EXPECT_EQ(il.size(), static_span.size());
  for (size_t i = 0; i < static_span.size(); ++i)
    EXPECT_EQ(il.begin()[i], static_span[i]);
}
