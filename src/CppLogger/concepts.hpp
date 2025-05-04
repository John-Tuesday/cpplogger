#pragma once

#include <iostream>
#include <ranges>
#include <source_location>
#include <string_view>

namespace logger {
enum class MessageType;
}

/**
 * @brief Constraints for logging helper classes.
 */
namespace logger::concepts {

/** Callable that returns a usable output stream. */
template <typename T>
concept ProvidesLogTarget = requires(T t) {
  { t() } -> std::common_reference_with<std::ostream &>;
};

/** Tests if each type of a tuple is a valid log target provider */
template <typename T> struct IsTupleProvider {
  template <std::size_t... Ns>
  static consteval bool test(std::integer_sequence<std::size_t, Ns...>) {
    if constexpr ((ProvidesLogTarget<std::tuple_element_t<Ns, T>> && ...))
      return true;
    return false;
  }

  /** `true` if all element types are log target providers */
  static constexpr bool value =
      test(std::make_index_sequence<std::tuple_size_v<T>>{});
};

/** Each type of a tuple is a valid log target provider */
template <typename T>
constexpr bool IsTupleProvider_v = IsTupleProvider<T>::value;

/** Each type of a tuple is a valid log target provider */
template <typename T>
concept LogTargetProviderTupleLike = IsTupleProvider_v<T>;

/** Range of objects with constraint `ProvidesLogTarget`. */
template <typename T>
concept LogTargetProviderRange =
    std::ranges::range<T> && ProvidesLogTarget<std::ranges::range_value_t<T>>;

/** Group of providers in a range or tuple-like object */
template <typename T>
concept LogTargetProviderGroup =
    LogTargetProviderRange<T> || LogTargetProviderTupleLike<T>;

/** Provides a range of log target provides. */
template <typename T, MessageType M>
concept IndirectlyProvidesLogTargets = requires(T t) {
  { t.template logproviders<M>() } noexcept -> LogTargetProviderGroup;
};

/** Provider group is a range */
template <typename T, MessageType M>
concept IndirectlyProvidesLogTargetsTupleLike = requires(T t) {
  { t.template logproviders<M>() } noexcept -> LogTargetProviderTupleLike;
};

/** Provider group is tuple-like */
template <typename T, MessageType M>
concept IndirectlyProvidesLogTargetsRange = requires(T t) {
  { t.template logproviders<M>() } noexcept -> LogTargetProviderRange;
};

/**
 * Meets the requirements of a log formatter.
 *
 * Should return iterator past the end of the output range.
 * Should essentially mirror the behavior of `std::format_to`.
 */
template <typename T, MessageType M>
concept IsLogFormatter = requires(T t) {
  {
    t.template format<M>(std::declval<std::source_location>(),
                         std::ostream_iterator<char>(std::cout),
                         std::declval<std::string_view>())
  } -> std::same_as<decltype(std::ostream_iterator<char>(std::cout))>;
};

/** Meets the requirements of a log filter. */
template <typename T, MessageType M>
concept IsLogFilter = requires(T t) {
  {
    t.template filter<M>(std::declval<std::source_location>())
  } noexcept -> std::same_as<bool>;
};

} // namespace logger::concepts
