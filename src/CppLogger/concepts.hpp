#pragma once

#include <iostream>
#include <source_location>
#include <syncstream>
#include <utility>

namespace logger {
struct LogContext;
} // namespace logger

/**
 * @brief Constraints for logging helper classes.
 */
namespace logger::concepts {

/**
 * Provides read access to log context.
 */
template <typename T>
concept LogContextFrom = requires(T t, LogContext &ctx) { ctx = t; };

/**
 * Context type which can be constructed from `std::source_location`.
 */
template <typename T>
concept ConstructibleLogContext =
    LogContextFrom<T> &&
    requires(std::source_location location) { T{location}; };

/**
 * Stream which can be used as an argument of type `std::ostream&`.
 */
template <typename T>
concept PrintableStream = !std::is_const_v<T> && requires(T t) {
  std::declval<void(std::ostream &)>()(t);
};

/**
 * Output device used when writing logs
 */
template <typename T>
concept LogTarget = requires(T t) { std::osyncstream{t}; };

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
concept PrintsToLog =
    requires(T t, std::ostream stream, const logger::LogContext &context,
             std::string_view message) {
      { t.print(stream, context, message) };
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
