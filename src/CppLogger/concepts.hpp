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

/** Range of objects with constraint `ProvidesLogTarget`. */
template <typename T>
concept LogTargetProviderRange =
    std::ranges::range<T> && ProvidesLogTarget<std::ranges::range_value_t<T>>;

/** Provides a range of log target provides. */
template <typename T, MessageType M>
concept IndirectlyProvidesLogTargets = requires(T t) {
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
