
#include <gtest/gtest.h>

#include "nanda/rank.hh"

TEST(RankTest, AllRank)
{

  EXPECT_EQ((nanda::rank(std::make_tuple(1, 2, 3))), 3);
  EXPECT_EQ((nanda::rank(std::array<size_t, 2>{})), 2);
  EXPECT_EQ((nanda::rank(std::array<int, 2>{})), 2);
}
