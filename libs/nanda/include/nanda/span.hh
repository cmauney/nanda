#ifndef NANDA_SPAN2_HEADER
#define NANDA_SPAN2_HEADER

#include <array>       // for std::array, etc.
#include <cassert>     // for assert
#include <cstddef>     // for std::size_t, etc.
#include <iterator>    // for std::reverse_iterator, etc.
#include <limits>      // for std::numeric_limits
#include <type_traits> // for std::enable_if, etc.

#include "concepts.hh"
#include "rank.hh"
#include "utility.hh"

namespace nanda {
// constants

namespace detail {
using span_index_t = std::ptrdiff_t;
}

constexpr detail::span_index_t dynamic_extent = -1;

namespace detail {
template<class To,
         class From,
         REQUIRES(std::is_integral_v<To>&& std::is_integral_v<From>)>
constexpr To
narrow_cast(From from) noexcept
{
  // TODO: some asserts here
  return static_cast<To>(from);
}

template<class T>
constexpr span_index_t
byte_size(span_index_t n) noexcept
{
  // TODO: some asserts here
  return n == dynamic_extent ? dynamic_extent
                             : n * narrow_cast<span_index_t>(sizeof(T));
}

template<span_index_t N>
struct span_extent
{
  constexpr span_extent(span_index_t size) noexcept
    // this constructor does nothing, the delegation exists only
    // to provide a place for the contract check expression.
    : span_extent{}
  {}

  constexpr span_index_t size() const noexcept { return N; }
};
template<>
struct span_extent<dynamic_extent>
{
  span_extent() = default;
  constexpr span_extent(span_index_t size) noexcept
    : size_{ size }
  {}
  constexpr span_index_t size() const noexcept { return size_; }

private:
  span_index_t size_ = 0;
};

constexpr span_index_t
subspan_extent(span_index_t Extent,
               span_index_t Offset,
               span_index_t Count) noexcept
{
  return Count == dynamic_extent && Extent != dynamic_extent ? Extent - Offset
                                                             : Count;
}

} // namespace detail

// class template span

template<class T, std::size_t N = detail::dynamic_extent>
class span;

// namespace detail {
// template<class Q>
// struct is_span_oracle : std::false_type
// {};

// template<class T, std::size_t Extent>
// struct is_span_oracle<span<T, Extent>> : std::true_type
// {};

// template<class Q>
// struct is_span : is_span_oracle<typename std::remove_cv<Q>::type>
// {};

// template<class Q>
// struct is_std_array_oracle : std::false_type
// {};

// template<class T, std::size_t Extent>
// struct is_std_array_oracle<std::array<T, Extent>> : std::true_type
// {};

// template<class Q>
// struct is_std_array : is_std_array_oracle<typename std::remove_cv<Q>::type>
// {};

// template<class Q>
// struct is_array : std::false_type
// {};

// template<class T>
// struct is_array<T[]> : std::true_type
// {};

// template<class T, std::size_t N>
// struct is_array<T[N]> : std::true_type
// {};

// template<class, class = void>
// struct has_size_and_data : std::false_type
// {};

// template<class C>
// struct has_size_and_data<C,
//                          std::void_t<decltype(std::size(std::declval<C>())),
//                                      decltype(std::data(std::declval<C>()))>>
//   : std::true_type
// {};

// template<class, class, class = void>
// struct is_compatible_element : std::false_type
// {};

// template<class C, class E>
// struct is_compatible_element<
//   C,
//   E,
//   std::void_t<decltype(std::data(std::declval<C>()))>>
//   : std::is_convertible<typename std::remove_pointer<decltype(
//                           std::data(std::declval<C&>()))>::type (*)[],
//                         E (*)[]>
// {};

// template<class C>
// struct is_container
//   : std::bool_constant<!is_span<C>::value && !is_array<C>::value &&
//                        !is_std_array<C>::value &&
//                        has_size_and_data<C>::value>
// {};

// template<class C, class E>
// struct is_compatible_container
//   : std::bool_constant<is_container<C>::value &&
//                        is_compatible_element<C, E>::value>
// {};

} // namespace nanda

namespace concepts {} // namespace concepts

#define span_REQUIRES_0(VA)                                                    \
  template<bool B = (VA), typename std::enable_if<B, int>::type = 0>

template<class T, detail::span_index_t N>
class span : detail::span_extent<N>
{
public:
  // constants and types
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using index_type = detail::span_index_t;
  using difference_type = index_type;
  using pointer = T*;
  using reference = T&;
  using iterator = T*;
  using reverse_iterator =
    std::reverse_iterator<iterator>; // ranges::reverse_iterator<iterator>;

  static constexpr index_type extent = N;

  // constructors, copy, and assignment
  constexpr span() noexcept = default;

  constexpr span(pointer ptr, index_type cnt) noexcept
    : detail::span_extent<N>{ cnt }
    , data_{ ptr }
  {}

  template<class = void> // Artificially templatize so that the other
                         // constructor is preferred for {ptr, 0}
  constexpr span(pointer first, pointer last) noexcept
    : span{ first, last - first }
  {}

  template<class Rng,
           REQUIRES(!std::is_same_v<span, remove_cvref_t<Rng>>),
           REQUIRES(CONCEPT(concepts::span_compatible_range<Rng, T>) &&
                    CONCEPT(concepts::span_dynamic_conversion<Rng, N>))>
  constexpr span(Rng&& rng) noexcept
    : span{ std::data(rng), detail::narrow_cast<index_type>(std::size(rng)) }
  {}

  template<class Rng,
           REQUIRES(!std::is_same_v<span, remove_cvref_t<Rng>>),
           REQUIRES(CONCEPT(concepts::span_compatible_range<Rng, T>) &&
                    CONCEPT(concepts::span_static_conversion<Rng, N>))>
  constexpr span(Rng&& rng) noexcept
    : span{ std::data(rng), N }
  {}

  template<index_t Cnt>
  constexpr span<T, Cnt> first() const noexcept
  {
    EXPECTS(Cnt <= size());
    return { data_, Cnt };
  }

  constexpr span<T, Cnt> first(index_type cnt) const noexcept
  {
    EXPECTS(cnt >= 0 && cnt <= size());
    EXPECTS(cnt == 0 || data_ != nullptr);
    return span<T>{ data_, cnt };
  }

  template<index_t Cnt>
  constexpr span<T, Cnt> last() const noexcept
  {
    static_assert(Cnt >= 0, "Count of elements to extract cannot be negative.");
    static_assert(
      N == dynamic_extent || Cnt <= N,
      "Count of elements to extract must be less than the static span extent.");

    EXPECTS(Cnt <= size());
    EXPECTS(Cnt == 0 || data_ != nullptr);
    return { data() + (size() - Cnt), Cnt };
  }
  constexpr span<T> last(index_type cnt) const noexcept
  {
    return EXPECTS(cnt >= 0 && cnt <= size()),
           EXPECTS((cnt == 0 && size() == 0) || data_ != nullptr),
           span<T>{ data_ + size() - cnt, cnt };
  }

  template<index_type Offset, index_type Count>
  constexpr span<T, detail::subspan_extent(N, Offset, Count)> subspan()
    const noexcept
  {
    static_assert(Offset >= 0,
                  "Offset of first element to extract cannot be negative.");
    static_assert(Count >= dynamic_extent,
                  "Count of elements to extract cannot be negative.");
    static_assert(
      N == dynamic_extent ||
        N >= Offset + (Count == dynamic_extent ? 0 : Count),
      "Sequence of elements to extract must be within the static span extent.");
    return EXPECTS(size() >= Offset + (Count == dynamic_extent ? 0 : Count)),
           EXPECTS((Offset == 0 && Count <= 0) || data_ != nullptr),
           span<T, detail::subspan_extent(N, Offset, Count)>{
             data_ + Offset, Count == dynamic_extent ? size() - Offset : Count
           };
  }
  template<index_type Offset>
  constexpr span<T, (N >= Offset ? N - Offset : dynamic_extent)> subspan()
    const noexcept
  {
    static_assert(Offset >= 0,
                  "Offset of first element to extract cannot be negative.");
    static_assert(
      N == dynamic_extent || N >= Offset,
      "Offset of first element to extract must be within the static "
      "span extent.");
    return EXPECTS(size() >= Offset),
           EXPECTS((Offset == 0 && size() == 0) || data_ != nullptr), span < T,
           N >= Offset ? N - Offset
                       : dynamic_extent > { data_ + Offset, size() - Offset };
  }
  constexpr span<T, dynamic_extent> subspan(index_type offset) const noexcept
  {
    return EXPECTS(offset >= 0), EXPECTS(size() >= offset),
           EXPECTS((offset == 0 && size() == 0) || data_ != nullptr),
           span<T, dynamic_extent>{ data_ + offset, size() - offset };
  }
  constexpr span<T, dynamic_extent> subspan(index_type offset,
                                            index_type cnt) const noexcept
  {
    return EXPECTS(offset >= 0), EXPECTS(cnt >= 0),
           EXPECTS(size() >= offset + cnt),
           EXPECTS((offset == 0 && cnt == 0) || data_ != nullptr),
           span<T, dynamic_extent>{ data_ + offset, cnt };
  }

  // observers
  constexpr pointer data() const noexcept { return data_; }

  using detail::span_extent<N>::size;
  constexpr index_type size_bytes() const noexcept
  {
    return size() * sizeof(T);
  }

  [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0; }

  // element access

  constexpr reference operator[](index_type idx) const noexcept
  {
    EXPECTS(idx < size());
    return *(data() + idx);
  }

  constexpr reference front() const noexcept
  {
    EXPECTS(!empty());
    return *data();
  }

  constexpr reference back() const noexcept
  {
    EXPECTS(!empty());
    return *(data() + (size() - 1));
  }

  constexpr pointer data() const noexcept { return data_; }

  // iterator support

  constexpr iterator begin() const noexcept { return data(); }

  constexpr iterator end() const noexcept { return data() + size(); }

  constexpr const_iterator cbegin() const noexcept { return data(); }

  constexpr const_iterator cend() const noexcept { return data() + size(); }

  constexpr reverse_iterator rbegin() const noexcept
  {
    return reverse_iterator{ end() };
  }

  constexpr reverse_iterator rend() const noexcept
  {
    return reverse_iterator{ begin() };
  }

  constexpr const_reverse_iterator crbegin() const noexcept
  {
    return reverse_iterator{ cend() };
  }

  constexpr const_reverse_iterator crend() const noexcept
  {
    return reverse_iterator{ cbegin() };
  }

  friend constexpr iterator begin(span s) noexcept { return s.begin(); }

  friend constexpr iterator end(span s) noexcept { return s.end(); }

  template<class U, index_type M, REQUIRES(equality_comparable_with<T, U>)>
  bool operator==(span<U, M> const& that) const
  {
    RANGES_EXPECT(!size() || data());
    RANGES_EXPECT(!that.size() || that.data());
    return ranges::equal(*this, that);
  }
  template<class U, index_type M, REQUIRES(equality_comparable_with<T, U>)>
  bool operator!=(span<U, M> const& that) const
  {
    return !(*this == that);
  }

  template<class U, index_type M, REQUIRES(totally_ordered_with<T, U>)>
  bool operator<(span<U, M> const& that) const
  {
    RANGES_EXPECT(!size() || data());
    RANGES_EXPECT(!that.size() || that.data());
    return ranges::lexicographical_compare(*this, that);
  }
  template<class U, index_type M, REQUIRES(totally_ordered_with<T, U>)>
  bool operator>(span<U, M> const& that) const
  {
    return that < *this;
  }
  template<class U, index_type M, REQUIRES(totally_ordered_with<T, U>)>
  bool operator<=(span<U, M> const& that) const
  {
    return !(that < *this);
  }
  template<class U, index_type M, REQUIRES(totally_ordered_with<T, U>)>
  bool operator>=(span<U, M> const& that) const
  {
    return !(*this < that);
  }

private:
  T* data_ = nullptr;
};

} // namespace nanda

template<class T, detail::span_index_t N>
RANGES_INLINE_VAR constexpr bool enable_borrowed_range<span<T, N>> = true;

template<class T, detail::span_index_t N>
constexpr detail::span_index_t span<T, N>::extent;

template<class Rng, REQUIRES(CONCEPT(concepts::contiguous_range<Rng>))>
span(Rng&& rng) -> span<
  detail::element_t<Rng>,
  (range_cardinality<Rng>::value < cardinality()
     ? dynamic_extent
     : static_cast<detail::span_index_t>(range_cardinality<Rng>::value))>;

namespace std {

// tuple interface
// the primary template declarations are included in <array>

template<class T, std::size_t N>
struct tuple_size<nanda::span<T, N>> : std::integral_constant<std::size_t, N>
{};

// not defined
template<class T>
struct tuple_size<nanda::span<T, nanda::dynamic_extent>>;

template<std::size_t I, class T, std::size_t N>
struct tuple_element<I, nanda::span<T, N>>
{
  static_assert(N != nanda::dynamic_extent && I < N);
  using type = T;
};

template<std::size_t I, class T, std::size_t N>
constexpr T&
get(nanda::span<T, N> s) noexcept
{
  static_assert(N != nanda::dynamic_extent && I < N);
  return s[I];
}

} // namespace std

#endif