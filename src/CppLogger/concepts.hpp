#pragma once

#include <iostream>
#include <source_location>
#include <utility>

namespace logger {
struct LogContext;
} // namespace logger

/**
 * @brief Constraints for logging helper classes.
 */
namespace logger::concepts {

template <typename T>
concept LogContextFrom = requires(T t, LogContext &ctx) { ctx = t; };

template <typename T>
concept ConstructibleLogContext =
    LogContextFrom<T> &&
    requires(std::source_location location) { T{location}; };

/**
 * Output device used when writing logs
 */
template <typename T>
concept LogTarget = std::constructible_from<std::osyncstream, T>;

/**
 * Tuple-Like type whose elements all satisfy `LogTarget`.
 */
template <typename T>
concept TupleLikeOfLogTargets = requires {
  std::apply([]<LogTarget... Args>(Args &&...) constexpr {}, std::declval<T>());
};

/**
 * Provides a collection of output targets in response to logging context.
 */
template <typename T>
concept ProvidesLogOutputTargets = requires(T t) {
  {
    t.targets(std::declval<logger::LogContext>())
  } -> logger::concepts::TupleLikeOfLogTargets;
};
;

/**
 * Provides a function to print a log messages.
 */
template <typename T>
concept PrintsToLog = requires(T t) {
  {
    t.print(std::declval<std::ostream &>(), std::declval<logger::LogContext>(),
            std::declval<std::string_view>())
  };
};

/**
 * Meets the requirements of a log filter.
 */
template <typename T>
concept FiltersLog = requires(T t) {
  {
    t.filter(std::declval<logger::LogContext>())
  } noexcept -> std::same_as<bool>;
};

} // namespace logger::concepts
