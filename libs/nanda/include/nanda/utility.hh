#ifndef NANDA_UTILITY_HEADER
#define NANDA_UTILITY_HEADER

#include <cstddef>
#include <tuple>
#include <type_traits>

#define EXPECTS(...) assert((__VA_ARGS__))

namespace nanda::detail {
template<class... Ts>
struct overloaded : Ts...
{
  using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
} // namespace nanda::detail

#define REQUIRES_METHOD(class_type, method)                                    \
  nanda::detail::overloaded{                                                   \
    [](auto* arg,                                                              \
       decltype(&std::decay_t<decltype(*arg)>::method) ptr) constexpr {        \
      return true;                                                             \
  }                                                                            \
  , [](auto* arg, ...) constexpr { return false; }                             \
  }                                                                            \
  ((class_type*)nullptr, nullptr)

namespace nanda {
// Trait to type check all types of a std::tuple
template<template<class, class...> class Trait, class Tuple>
struct all_of;

template<template<class, class...> class Trait, class... Types>
struct all_of<Trait, std::tuple<Types...>> : std::conjunction<Trait<Types>...>
{};

template<template<class, class...> class Trait, class... Types>
struct all_of<Trait, std::pair<Types...>> : std::conjunction<Trait<Types>...>
{};

template<class T, template<class, auto> class TT>
struct is_template_instantiation : std::false_type
{};
template<template<class, auto> class TT, class U, auto N>
struct is_template_instantiation<TT<U, N>, TT> : std::true_type
{};

template<class T>
using iter_reference_t = decltype(*std::declval<T&>());

template<class T>
struct remove_cvref
{
  typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template<class T>
using remove_cvref_t = typename remove_cvref<T>::type;

namespace {
template<typename Ptr, typename = void>
struct has_std_to_address : std::false_type
{};
template<typename Ptr>
struct has_std_to_address<
  Ptr,
  std::void_t<decltype(std::pointer_traits<Ptr>::to_address(
    std::declval<Ptr>()))>> : std::true_type
{};
} // namespace
//
// Reference: https://wg21.link/pointer.conversion#lib:to_address
template<typename T>
constexpr T*
to_address(T* p) noexcept
{
  static_assert(!std::is_function<T>::value,
                "Error: T must not be a function type.");
  return p;
}
template<typename Ptr>
constexpr auto
to_address(const Ptr& p) noexcept
{
  if constexpr (has_std_to_address<Ptr>::value) {
    return std::pointer_traits<Ptr>::to_address(p);
  } else {
    return nanda::to_address(p.operator->());
  }
}

#ifdef DEBUG
constexpr void
runtime_assert(bool condition, const char* msg)
{
  if (!condition)
    throw std::runtime_error(msg);
}
#else
constexpr void
runtime_assert([[maybe_unused]] bool condition,
               [[maybe_unused]] const char* msg)
{}
#endif

/// @brief Converts all tuple elements to an array of elements of the same size
/// @tparam tuple_t the type of the tuple to convert
/// @param tuple the input tuple to convert
/// @return std::array of the tuple elements
template<class tuple_t>
constexpr auto
tuple_to_array(tuple_t&& tuple)
{

  constexpr auto get_array = [](auto&&... x) constexpr
  {
    return std::array{ std::forward<decltype(x)>(x)... };
  };
  return std::apply(get_array, std::forward<tuple_t>(tuple));
}

} // namespace nanda
#endif