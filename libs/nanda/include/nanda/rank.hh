#ifndef NANDA_RANK_HEADER
#define NANDA_RANK_HEADER

#include <cstddef>
#include <type_traits>

#include "index_types.hh"
#include "utility.hh"

namespace nanda {

template<class T>
struct Rank
{
  static constexpr std::size_t value = T::rank();
};

template<std::size_t N, class Idx>
struct Rank<std::array<Idx, N>>
{
  static_assert(std::is_integral<Idx>::value, "Not an integer array.");
  static constexpr std::size_t value = N;
};

template<class... Idx>
struct Rank<std::tuple<Idx...>>
{

private:
  using my_type = std::tuple<Idx...>;

public:
  static_assert(all_of<std::is_integral, my_type>::value,
                "Not an integer tuple.");
  static constexpr std::size_t value = sizeof...(Idx);
};

template<class... Idx>
struct Rank<std::pair<Idx...>>
{

private:
  using my_type = std::pair<Idx...>;

public:
  static_assert(all_of<std::is_integral, my_type>::value,
                "Not an integer pair.");
  static constexpr std::size_t value = 2;
};

/// @brief Queries the _static_ rank of the input type T
/// @tparam T the type to query the static rank of_
/// @return static rank of the input type T
template<class T>
constexpr std::size_t
rank(const T&)
{
  return Rank<T>::value;
}

} // namespace nanda

#endif