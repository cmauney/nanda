
#include <gtest/gtest.h>

#include "nanda/index_algos.hh"

using namespace nanda;

TEST(IndexAlgosTest, TestGetShift1D)
{
  size_type n0 = 10;
  std::array<size_type, 1> dims{ n0 };

  EXPECT_EQ((get_shift<0, StorageOrder::RowMajor>(dims)), 1);
  EXPECT_EQ((get_shift<0, StorageOrder::ColMajor>(dims)), 1);
}

TEST(IndexAlgosTest, TestGetShift2D)
{
  size_type n0 = 10;
  size_type n1 = 11;
  std::array<size_type, 2> dims{ n0, n1 };

  //[nj, ni]
  EXPECT_EQ((get_shift<0, StorageOrder::RowMajor>(dims)), n1); // ni
  EXPECT_EQ((get_shift<1, StorageOrder::RowMajor>(dims)), 1);

  //[ni, nj]
  EXPECT_EQ((get_shift<0, StorageOrder::ColMajor>(dims)), 1);
  EXPECT_EQ((get_shift<1, StorageOrder::ColMajor>(dims)), n0); // ni
}

TEST(IndexAlgosTest, TestGetShift3D)
{

  size_type n0 = 10;
  size_type n1 = 1;
  size_type n2 = 12;
  std::array<size_type, 3> dims{ n0, n1, n2 };

  EXPECT_EQ((get_shift<0, StorageOrder::RowMajor>(dims)), n1 * n2);
  EXPECT_EQ((get_shift<1, StorageOrder::RowMajor>(dims)), n2);
  EXPECT_EQ((get_shift<2, StorageOrder::RowMajor>(dims)), 1);

  EXPECT_EQ((get_shift<0, StorageOrder::ColMajor>(dims)), 1);
  EXPECT_EQ((get_shift<1, StorageOrder::ColMajor>(dims)), n0);
  EXPECT_EQ((get_shift<2, StorageOrder::ColMajor>(dims)), n0 * n1);
}

TEST(IndexAlgosTest, TestFlatten1D)
{

  using dimension = std::array<size_type, 1>;
  using position = std::array<index_type, 1>;
  dimension dim{ 12 };
  position pos;

  pos = { 3 };
  EXPECT_EQ((flatten<StorageOrder::RowMajor>(pos, dim)), 3);
  EXPECT_EQ((flatten<StorageOrder::ColMajor>(pos, dim)), 3);

  pos = { 0 };
  EXPECT_EQ((flatten<StorageOrder::RowMajor>(pos, dim)), 0);
  EXPECT_EQ((flatten<StorageOrder::ColMajor>(pos, dim)), 0);

  pos = { 11 };
  EXPECT_EQ((flatten<StorageOrder::RowMajor>(pos, dim)), 11);
  EXPECT_EQ((flatten<StorageOrder::ColMajor>(pos, dim)), 11);

#ifdef DEBUG
  pos = { 12 };
  EXPECT_ANY_THROW((flatten<StorageOrder::RowMajor>(pos, dim)));
  EXPECT_ANY_THROW((flatten<StorageOrder::ColMajor>(pos, dim)));
#endif
}

TEST(IndexAlgosTest, TestFlatten2D)
{

  using dimension = std::array<size_type, 2>;
  using position = std::array<index_type, 2>;
  dimension dim{ 4, 5 };

  position pos;

  pos = { 0, 0 };
  EXPECT_EQ((flatten<StorageOrder::RowMajor>(pos, dim)), 0);
  EXPECT_EQ((flatten<StorageOrder::ColMajor>(pos, dim)), 0);

  pos = { 1, 1 };
  EXPECT_EQ((flatten<StorageOrder::RowMajor>(pos, dim)), 6);
  EXPECT_EQ((flatten<StorageOrder::ColMajor>(pos, dim)), 5);

#ifdef DEBUG
  pos = { 5, 1 };
  EXPECT_ANY_THROW((flatten<StorageOrder::RowMajor>(pos, dim)));
  EXPECT_ANY_THROW((flatten<StorageOrder::ColMajor>(pos, dim)));
#endif
}

TEST(IndexAlgosTest, TestFlatten3D)
{

  using dimension = std::array<size_type, 3>;
  using position = std::array<index_type, 3>;

  dimension dim{ 4, 1, 6 };
  position pos;

  pos = { 0, 0, 0 };
  EXPECT_EQ((flatten<StorageOrder::RowMajor>(pos, dim)), 0);
  EXPECT_EQ((flatten<StorageOrder::ColMajor>(pos, dim)), 0);

  pos = { 1, 0, 3 };
  EXPECT_EQ((flatten<StorageOrder::RowMajor>(pos, dim)), 9);
  EXPECT_EQ((flatten<StorageOrder::ColMajor>(pos, dim)), 13);

#ifdef DEBUG
  pos = { 1, 1, 3 };
  EXPECT_ANY_THROW((flatten<StorageOrder::RowMajor>(pos, dim)));
  EXPECT_ANY_THROW((flatten<StorageOrder::ColMajor>(pos, dim)));
#endif
}

TEST(IndexAlgosTest, TestFlatten4D)
{

  using dimension = std::array<size_type, 4>;
  using position = std::array<index_type, 4>;

  dimension dim{ 4, 1, 6, 5 };
  position pos;

  pos = { 0, 0, 0, 0 };
  EXPECT_EQ((flatten<StorageOrder::RowMajor>(pos, dim)), 0);
  EXPECT_EQ((flatten<StorageOrder::ColMajor>(pos, dim)), 0);

  pos = { 1, 0, 3, 2 };
  EXPECT_EQ((flatten<StorageOrder::RowMajor>(pos, dim)), 47);
  EXPECT_EQ((flatten<StorageOrder::ColMajor>(pos, dim)), 61);

#ifdef DEBUG
  pos = { 1, 1, 3, 4 };
  EXPECT_ANY_THROW((flatten<StorageOrder::RowMajor>(pos, dim)));
  EXPECT_ANY_THROW((flatten<StorageOrder::ColMajor>(pos, dim)));
#endif
}

TEST(IndexAlgosTest, TestUnflatten1D)
{

  using dimension = std::array<size_type, 1>;
  using position = std::array<index_type, 1>;

  index_type pos;
  dimension dim{ 7 };

  pos = 0;
  EXPECT_EQ((unflatten<StorageOrder::RowMajor>(pos, dim)), (position{ 0 }));
  EXPECT_EQ((unflatten<StorageOrder::ColMajor>(pos, dim)), (position{ 0 }));

  pos = 3;
  EXPECT_EQ((unflatten<StorageOrder::RowMajor>(pos, dim)), (position{ 3 }));
  EXPECT_EQ((unflatten<StorageOrder::ColMajor>(pos, dim)), (position{ 3 }));

#ifdef DEBUG
  pos = 7;
  EXPECT_ANY_THROW((unflatten<StorageOrder::RowMajor>(pos, dim)));
  EXPECT_ANY_THROW((unflatten<StorageOrder::ColMajor>(pos, dim)));
#endif
}

TEST(IndexAlgosTest, TestUnflatten2D)
{
  using dimension = std::array<size_type, 2>;
  using position = std::array<index_type, 2>;
  dimension dim{ 3, 6 };
  index_type pos;

  pos = 0;
  EXPECT_EQ((unflatten<StorageOrder::RowMajor>(pos, dim)), (position{ 0, 0 }));
  EXPECT_EQ((unflatten<StorageOrder::ColMajor>(pos, dim)), (position{ 0, 0 }));

  pos = 4;
  EXPECT_EQ((unflatten<StorageOrder::RowMajor>(pos, dim)), (position{ 0, 4 }));
  EXPECT_EQ((unflatten<StorageOrder::ColMajor>(pos, dim)), (position{ 1, 1 }));

  pos = 7;
  EXPECT_EQ((unflatten<StorageOrder::RowMajor>(pos, dim)), (position{ 1, 1 }));
  EXPECT_EQ((unflatten<StorageOrder::ColMajor>(pos, dim)), (position{ 1, 2 }));

  pos = 16;
  EXPECT_EQ((unflatten<StorageOrder::RowMajor>(pos, dim)), (position{ 2, 4 }));
  EXPECT_EQ((unflatten<StorageOrder::ColMajor>(pos, dim)), (position{ 1, 5 }));

#ifdef DEBUG
  pos = 18;
  EXPECT_ANY_THROW((flatten<StorageOrder::RowMajor>(pos, dim)));
  EXPECT_ANY_THROW((flatten<StorageOrder::ColMajor>(pos, dim)));
#endif
}

TEST(IndexAlgosTest, TestUnflatten3D)
{
  using dimension = std::array<size_type, 3>;
  using position = std::array<index_type, 3>;
  dimension dim{ 3, 1, 8 };
  index_type pos;

  pos = 0;
  EXPECT_EQ((unflatten<StorageOrder::RowMajor>(pos, dim)),
            (position{ 0, 0, 0 }));
  EXPECT_EQ((unflatten<StorageOrder::ColMajor>(pos, dim)),
            (position{ 0, 0, 0 }));

  pos = 6;
  EXPECT_EQ((unflatten<StorageOrder::RowMajor>(pos, dim)),
            (position{ 0, 0, 6 }));
  EXPECT_EQ((unflatten<StorageOrder::ColMajor>(pos, dim)),
            (position{ 0, 0, 2 }));

  pos = 1;
  EXPECT_EQ((unflatten<StorageOrder::RowMajor>(pos, dim)),
            (position{ 0, 0, 1 }));
  EXPECT_EQ((unflatten<StorageOrder::ColMajor>(pos, dim)),
            (position{ 1, 0, 0 }));

  pos = 23;
  EXPECT_EQ((unflatten<StorageOrder::RowMajor>(pos, dim)),
            (position{ 2, 0, 7 }));
  EXPECT_EQ((unflatten<StorageOrder::ColMajor>(pos, dim)),
            (position{ 2, 0, 7 }));

#ifdef DEBUG
  pos = 24;
  EXPECT_ANY_THROW((flatten<StorageOrder::RowMajor>(pos, dim)));
  EXPECT_ANY_THROW((flatten<StorageOrder::ColMajor>(pos, dim)));
#endif
}

TEST(IndexAlgosTest, TestUnflatten4D)
{
  using dimension = std::array<size_type, 4>;
  using position = std::array<index_type, 4>;
  dimension dim{ 3, 1, 8, 1 };
  index_type pos;

  pos = 0;
  EXPECT_EQ((unflatten<StorageOrder::RowMajor>(pos, dim)),
            (position{ 0, 0, 0, 0 }));
  EXPECT_EQ((unflatten<StorageOrder::ColMajor>(pos, dim)),
            (position{ 0, 0, 0, 0 }));

  pos = 2;
  EXPECT_EQ((unflatten<StorageOrder::RowMajor>(pos, dim)),
            (position{ 0, 0, 2, 0 }));
  EXPECT_EQ((unflatten<StorageOrder::ColMajor>(pos, dim)),
            (position{ 2, 0, 0, 0 }));

  pos = 3;
  EXPECT_EQ((unflatten<StorageOrder::RowMajor>(pos, dim)),
            (position{ 0, 0, 3, 0 }));
  EXPECT_EQ((unflatten<StorageOrder::ColMajor>(pos, dim)),
            (position{ 0, 0, 1, 0 }));

  pos = 23;
  EXPECT_EQ((unflatten<StorageOrder::RowMajor>(pos, dim)),
            (position{ 2, 0, 7, 0 }));
  EXPECT_EQ((unflatten<StorageOrder::ColMajor>(pos, dim)),
            (position{ 2, 0, 7, 0 }));

#ifdef DEBUG
  pos = 24;
  EXPECT_ANY_THROW((flatten<StorageOrder::RowMajor>(pos, dim)));
  EXPECT_ANY_THROW((flatten<StorageOrder::ColMajor>(pos, dim)));
#endif
}
