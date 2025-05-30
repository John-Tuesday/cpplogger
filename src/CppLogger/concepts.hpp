#pragma once

#include <iostream>
#include <ranges>
#include <source_location>
#include <utility>

namespace logger {
enum class MessageType;
}

/**
 * @brief Constraints for logging helper classes.
 */
namespace logger::concepts {

template <typename T>
concept LogTarget = std::constructible_from<std::osyncstream, T>;

template <typename T>
concept TupleLikeOfLogTargets = requires {
  std::apply([]<LogTarget... Args>(Args &&...) constexpr {}, std::declval<T>());
};

template <typename T, MessageType MType>
concept ProvidesLogOutputTargets = requires(T t) {
  {
    t.template targets<MType>(std::declval<const std::source_location &>())
  } -> TupleLikeOfLogTargets;
};

/** Callable that returns a usable output stream. */
template <typename T>
concept ProvidesLogTarget = requires(T t) {
  { t() } -> std::common_reference_with<std::ostream &>;
};

/**
 * Tuple-like struct of log target providers.
 */
template <typename T>
concept TupleLikeLogTargetProviders = requires(T t) {
  {
    std::apply([]<typename... Args>
                 requires(ProvidesLogTarget<Args> && ...)
               (Args &&...) constexpr { return; },
               t)
  };
};

/** Range of objects with constraint `ProvidesLogTarget`. */
template <typename T>
concept LogTargetProviderRange =
    std::ranges::range<T> && ProvidesLogTarget<std::ranges::range_value_t<T>>;

/** Provides a range of log target provides. */
template <typename T, MessageType M>
concept IndirectlyProvidesLogTargets = requires(T t) {
  { t.template providers<M>() } noexcept;
  requires requires(decltype(t.template providers<M>()) r) {
    requires(LogTargetProviderRange<decltype(r)> ||
             TupleLikeLogTargetProviders<decltype(r)>);
  };
};

/**
 * Provides a function to print a log messages.
 */
template <typename T, MessageType MType>
concept PrintsToLog = requires(T t) {
  {
    t.template print<MType>(std::declval<std::ostream &>(),
                            std::declval<std::source_location>(), "message")
  };
};

/** Meets the requirements of a log filter. */
template <typename T, MessageType M>
concept FiltersLog = requires(T t) {
  {
    t.template filter<M>(std::declval<std::source_location>())
  } noexcept -> std::same_as<bool>;
};

} // namespace logger::concepts
