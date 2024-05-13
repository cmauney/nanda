#ifndef NANDA_EXTENTS_HEADER
#define NANDA_EXTENTS_HEADER

#include <cstddef>
#include <tuple>
#include <type_traits>

#define REQUIRES(...) typename std::enable_if<__VA_ARGS__, bool>::type = false

template <template <class...> class C, class... T>
constexpr bool CONCEPT_V = C<T...>::value;

namespace nanda::concepts::expr
{
  
struct nonesuch {
  ~nonesuch() = delete;
  nonesuch(nonesuch const&) = delete;
  void operator=(nonesuch const&) = delete;
};

namespace detail {

template <class Default, class AlwaysVoid, template <class...> class Op,
        class... Args>
struct detector {
  using value_t = std::false_type;
  using type = Default;
};

template <class Default, template <class...> class Op, class... Args>
struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
  using value_t = std::true_type;
  using type = Op<Args...>;
};

}  // namespace detail

template <template <class...> class Op, class... Args>
using is_detected =
  typename detail::detector<nonesuch, void, Op, Args...>::value_t;

template <template <class...> class Op, class... Args>
using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

template <class Default, template <class...> class Op, class... Args>
using detected_or = detail::detector<Default, void, Op, Args...>;

template <template <class...> class Op, class... Args>
constexpr inline bool is_detected_v = is_detected<Op, Args...>::value;

template <class Default, template <class...> class Op, class... Args>
using detected_or_t = typename detected_or<Default, Op, Args...>::type;

template <class Expected, template <class...> class Op, class... Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

template <class Expected, template <class...> class Op, class... Args>
constexpr inline bool is_detected_exact_v =
  is_detected_exact<Expected, Op, Args...>::value;

template <class To, template <class...> class Op, class... Args>
using is_detected_convertible =
  std::is_convertible<detected_t<Op, Args...>, To>;
template <class To, template <class...> class Op, class... Args>

constexpr inline bool is_detected_convertible_v =
  is_detected_convertible<To, Op, Args...>::value;

}

namespace nanda::concepts
{
 
  namespace detail {

  template<class From, class To>
  using convertible_to_frag_ = decltype(static_cast<std::decltype<To>(std::decltype<From>()));
  
  template<class T>
  using bool_testable_frag_ = decltype(!std::declval<T&&>());

  } // detail

  template<class From,
           class To,
           REQUIRES(std::is_convertible_v<From, To>),
           REQUIRES(is_detected_v<convertible_to_frag_, From, To>)>
  struct convertible_to;

  template<class T>
  using boolean_testable_impl_ = decltype(convertible_to<T, bool>);

  template<class T,
           REQUIRES(is_detected_v<boolean_testable_frag_, T>) &&
                    is_detected_v<boolean_testable_impl_, T>)>
  struct boolean_testable_
  {};

  /// \concept boolean_testable_
  /// \brief The \c boolean_testable_ concept
  template<typename T>
  CPP_concept boolean_testable_ =
    CPP_requires_ref(boolean_testable_frag_, T) && boolean_testable_impl_<T>;

  /// \concept weakly_equality_comparable_with_frag_
  /// \brief The \c weakly_equality_comparable_with_frag_ concept
  template<class T, class U, REQUIRES()>

  CPP_requires(weakly_equality_comparable_with_frag_,
               requires(detail::as_cref_t<T> t, detail::as_cref_t<U> u) //
               (concepts::requires_<boolean_testable_<decltype(t == u)>>,
                concepts::requires_<boolean_testable_<decltype(t != u)>>,
                concepts::requires_<boolean_testable_<decltype(u == t)>>,
                concepts::requires_<boolean_testable_<decltype(u != t)>>));
  /// \concept weakly_equality_comparable_with_
  /// \brief The \c weakly_equality_comparable_with_ concept
  template<typename T, typename U>
  CPP_concept weakly_equality_comparable_with_ =
    CPP_requires_ref(weakly_equality_comparable_with_frag_, T, U);
  } // namespace detail

  template<typename Rng>
  using data_t = decltype(std::data(std::declval<Rng&>()));

  template<typename Rng>
  using element_t = std::remove_pointer_t<data_t<Rng>>;

  template<class, class = void>
  struct has_size_and_data : std::false_type
  {};

  template<class C>
  struct has_size_and_data<C,
                           std::void_t<decltype(std::size(std::declval<C>())),
                                       decltype(std::data(std::declval<C>()))>>
    : std::true_type
  {};

  template<class Rng,
           class T,
           REQUIRES(std::is_convertible_v<element_t<Rng>(*)[], T(*)[]>)>
  struct span_compatible_range_;

  template<class Rng,
           class T,
           REQUIRES(CONCEPT(has_size_and_data<Rng>)),
           REQUIRES(CONCEPT(span_compatible_range_<Rng, T>))>
  struct span_compatible_range;

  template<class Rng, detail::span_index_t N, REQUIRES(N == dynamic_extent)>
  struct span_dynamic_conversion;

  template<class Rng,
           detail::span_index_t N,
           REQUIRES(N != dynamic_extent && Rank<Rng>::value == N)>
  struct span_static_conversion;

  template<class T,
           REQUIRES(contiguous_iterator<iterator_t<T>>),
           REQUIRES(same_as < detail::data_t<T>,
                    std::add_pointer_t<iter_reference_t<iterator_t<T>>>),
           REQUIRES(random_access_)>
  struct contiguous_range_;

  /// \concept equality_comparable
  /// \brief The \c equality_comparable concept
  template<class T, REQUIRES(detail::weakly_equality_comparable_with_<T, T>)>
  struct equality_comparable;

  /// \concept equality_comparable_with_
  /// \brief The \c equality_comparable_with_ concept
  template<class T,
           class U,
           REQUIRES(
             equality_comparable <
             common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>)>
  struct equality_comparable_with_;

  /// \concept equality_comparable_with
  /// \brief The \c equality_comparable_with concept
  template<typename T, typename U>
  CPP_concept equality_comparable_with = equality_comparable<T>&&
    equality_comparable<U>&& detail::weakly_equality_comparable_with_<T, U>&&
      common_reference_with<detail::as_cref_t<T>, detail::as_cref_t<U>>&&
        CPP_concept_ref(concepts::equality_comparable_with_, T, U);

  /// \concept totally_ordered
  /// \brief The \c totally_ordered concept
  template<typename T>
  CPP_concept totally_ordered =
    equality_comparable<T>&& detail::partially_ordered_with_<T, T>;

  /// \concept totally_ordered_with_
  /// \brief The \c totally_ordered_with_ concept
  CPP_template(typename T, typename U)(
    concept(totally_ordered_with_)(T, U),
    totally_ordered<
      common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>> CPP_and
      detail::partially_ordered_with_<T, U>);

  /// \concept totally_ordered_with
  /// \brief The \c totally_ordered_with concept
  template<typename T, typename U>
  CPP_concept totally_ordered_with =
    totally_ordered<T>&& totally_ordered<U>&& equality_comparable_with<T, U>&&
      CPP_concept_ref(concepts::totally_ordered_with_, T, U);

  // equality_comparable_with
  // totally_ordered_with
  // range_cardinality
} // namespace nanda::concepts

#endif // NANDA_EXTENS_HEADER