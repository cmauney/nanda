#ifndef NANDA_INDEX_ALGOS_HEADER
#define NANDA_INDEX_ALGOS_HEADER

#include <algorithm>
#include <numeric>

#include "rank.hh"

namespace nanda {

enum class StorageOrder
{
  RowMajor, // C-style
  ColMajor  // FORTRAN-style
};

///@brief Get the shift between consequtive indices in direction I based on the
/// storage order
///
///@tparam I the component (direction) to get the shift for
///@tparam storage the storage order
///@param dimension the maximum extent of the multidimensional array
///@return constexpr idx_t the shift in direction I
template<size_t I, StorageOrder storage, class Dim>
static constexpr std::size_t
get_shift(Dim dim)
{

  static_assert(I < rank(dim), "Shift index out of bounds");

  if constexpr (storage == StorageOrder::RowMajor) {

    return std::accumulate(std::begin(dim) + I + 1,
                           std::end(dim),
                           std::size_t(1),
                           std::multiplies<std::size_t>{});
  }

  return std::accumulate(std::begin(dim),
                         std::begin(dim) + I,
                         std::size_t(1),
                         std::multiplies<std::size_t>{});
}

template<StorageOrder storage, class Dim, std::size_t... Is>
static constexpr auto
get_shifts(Dim dim, std::index_sequence<Is...>)
{
  return std::array<std::size_t, Rank<decltype(dim)>::value>{
    get_shift<Is, storage>(dim)...
  };
}

///@brief Get the shifts between consequtive indices in all N directions of a
/// multidimensional (flattened) array
///
///@tparam storage storage order
///@param dimension the maximum extent of the multidimensional array
///@return constexpr std::array<idx_t, N> the shifts in all N directions
template<StorageOrder storage, class Dim>
static constexpr auto
get_shifts(Dim dim)
{
  return get_shifts<storage>(dim, std::make_index_sequence<rank(dim)>{});
}

///@brief Given an array of multidimensional indices ([k,j,i] for example)
/// computes the flat index based on the storage order, dimensions and
/// precomputed shifts in each direction
///
///@tparam storage storage order
///@param idx array of indices to flatten
///@param dim the maximum extent of the multidimensional array
///@param mult precomputed shifts of the dimensions
///@return constexpr index_type the flat index
template<class Idx, class Dim, class Mult>
static constexpr index_type
fast_flatten(Idx idx, Dim dim, Mult mult)
{
  // runtime_assert(indices_in_bounds(idx, dim), "Index out of bounds");
  return std::inner_product(
    std::begin(idx), std::end(idx), std::begin(mult), index_type(0));
}

///@brief Given an array of multidimensional indices ([k,j,i] for example)
/// computes the flat index based on the storage order and dimensions
///
///@tparam storage storage order
///@param idx array of indices to flatten
///@param dim the maximum extent of the multidimensional array
///@return constexpr index_type the flat index
template<StorageOrder storage, class Idx, class Dim>
static constexpr index_type
flatten(Idx idx, Dim dim)
{
  return fast_flatten(idx, dim, get_shifts<storage>(dim));
}

///@brief Given a flat index 'idx', dimensions 'dims' and precomputed shifts,
/// computes the unflattened (multidimensional) index.
///
///@tparam storage storage order
///@param idx index to unflatten
///@param dims maximum extent of the multidimensional indices
///@return constexpr std::array<index_type, N> array of multidimensional indices
template<StorageOrder storage, class Dims, class Mult>
static constexpr auto
fast_unflatten(index_type idx, Dims dims, Mult mult)
{

  runtime_assert(std::accumulate(std::begin(dims),
                                 std::end(dims),
                                 index_type(1),
                                 std::multiplies<index_type>{}) > idx,
                 "Index out of bounds");

  std::array<index_type, rank(dims)> md_idx;
  // TODO: this is not nice, try to make some sense at some point
  if constexpr (storage == StorageOrder::RowMajor) {

    for (size_t i = 0; i < rank(dims); ++i) {
      md_idx[i] = idx / index_type(mult[i]);
      idx -= md_idx[i] * index_type(mult[i]);
    }
  }

  else {

    for (size_t i = rank(dims) - 1; int(i) >= 0; --i) {
      md_idx[i] = idx / index_type(mult[i]);
      idx -= md_idx[i] * index_type(mult[i]);
    }
  }

  return md_idx;
}
///@brief Given dimensions and an index returns the multidimensional index based
/// on the storage order
///
///@tparam storage storage order
///@param idx index to unflatten
///@param dims maximum extent of the multidimensional indices
///@return constexpr std::array<idx_t, N> array of multidimensional indices
template<StorageOrder storage, class Dims>
static constexpr auto
unflatten(index_type idx, Dims dims)
{

  return fast_unflatten<storage>(idx, dims, get_shifts<storage>(dims));
}
} // namespace nanda

#endif