#ifndef NANDA_EXTENTS_HEADER
#define NANDA_EXTENTS_HEADER

namespace nanda {

template<class IndexType, std::size_t... Extents>
class extents
{
public:
  using index_type = IndexType;
  using size_type = std::make_unsigned_t<index_type>;
  using rank_type = std::size_t;

  // [mdspan.extents.obs], observers of the multidimensional index space
  static constexpr rank_type rank() noexcept { return sizeof...(Extents); }
  static constexpr rank_type rank_dynamic() noexcept
  {
    return dynamic - index(rank());
  }
  static constexpr std::size_t static_extent(rank_type) noexcept;
  constexpr index_type extent(rank_type) const noexcept;

  // [mdspan.extents.cons], constructors
  constexpr extents() noexcept = default;

  template<class OtherIndexType, std::size_t... OtherExtents>
  constexpr explicit(see below)
    extents(const extents<OtherIndexType, OtherExtents...>&) noexcept;
  template<class... OtherIndexTypes>
  constexpr explicit extents(OtherIndexTypes...) noexcept;
  template<class OtherIndexType, std::size_t N>
  constexpr explicit(N != rank_dynamic())
    extents(span<OtherIndexType, N>) noexcept;
  template<class OtherIndexType, std::size_t N>
  constexpr explicit(N != rank_dynamic())
    extents(const array<OtherIndexType, N>&) noexcept;

  // [mdspan.extents.cmp], comparison operators
  template<class OtherIndexType, std::size_t... OtherExtents>
  friend constexpr bool operator==(
    const extents&,
    const extents<OtherIndexType, OtherExtents...>&) noexcept;

  // [mdspan.extents.expo], exposition-only helpers
  //   constexpr std::size_t fwd - prod - of -
  //     extents(rank_type) const noexcept; // exposition only
  //   constexpr std::size_t rev - prod - of -
  //     extents(rank_type) const noexcept; // exposition only
  //   template<class OtherIndexType>
  //     static constexpr auto index -
  //     cast(OtherIndexType&&) noexcept; // exposition only

private:
  //   static constexpr rank_type dynamic -
  //     index(rank_type) noexcept; // exposition only
  //   static constexpr rank_type dynamic - index -
  //     inv(rank_type) noexcept;                             // exposition only
  //   array<index_type, rank_dynamic()> dynamic - extents{}; // exposition only
};

// template<class... Integrals>
// explicit extents(Integrals...)->see below;
} // namespace nanda

#endif