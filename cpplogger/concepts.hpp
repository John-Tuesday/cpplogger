#pragma once

#ifndef CPPLOGGER_CONCEPTS_HPP
#define CPPLOGGER_CONCEPTS_HPP

#include <iostream>
#include <syncstream>
#include <utility>

namespace logger {

template <typename CharT>
struct LogContext;

/**
 * Deduced associated character type of a given context.
 *
 * `T` is expected to be at least of type `LogContext`, but it is not checked.
 */
template <typename T>
using LogContextCharType = typename std::remove_cvref_t<T>::CharType;

}  // namespace logger

/**
 * @brief Constraints for logging helper classes.
 */
namespace logger::concepts {

/**
 * Provides read access to log context.
 */
template <typename T, typename CharT>
concept LogContextFrom =
    std::same_as<LogContextCharType<T>, CharT> &&
    requires(T t) { ([](const logger::LogContext<CharT>&) {})(t); };

/**
 * Convenience for `logger::concepts::LogContextFrom<T,
 * logger::LogContextCharType<T>>`
 *
 * @see LogContextFrom
 */
template <typename T>
concept LogContextLike = LogContextFrom<T, LogContextCharType<T>>;

/**
 * Context type which can be constructed from a corresponding
 * `logger::LogContext<CharT>` object.
 */
template <typename T, typename CharT>
concept ConstructibleLogContext = LogContextFrom<T, CharT> && requires() {
  T{std::declval<LogContext<CharT>>()};
};

/**
 * Convenience for `logger::concepts::ConstructibleLogContext<T,
 * logger::LogContextCharType<T>>`
 *
 * @see ConstructibleLogContext
 */
template <typename T>
concept ConstructibleLogContextLike =
    ConstructibleLogContext<T, LogContextCharType<T>>;

/**
 * Stream which can be used as an argument of type `std::basic_ostream<CharT>&`.
 */
template <typename T, typename CharT>
concept PrintableStream = !std::is_const_v<T> && requires(T t) {
  std::declval<void(std::basic_ostream<CharT>&)>()(t);
};

/**
 * Output device used when writing logs.
 */
template <typename T, typename CharT>
concept LogTarget = requires(T t) { std::basic_osyncstream<CharT>{t}; };

/**
 * Tuple-Like type whose elements all satisfy `LogTarget`.
 */
template <typename T, typename CharT>
concept TupleLikeOfLogTargets = requires {
  std::apply([]<LogTarget<CharT>... Args>(Args&&...) {}, std::declval<T>());
};

/**
 * Provides a collection of output targets in response to logging context.
 */
template <typename T, typename CharT>
concept ProvidesLogOutputTargets =
    requires(T t, const logger::LogContext<CharT>& context) {
      { t.targets(context) } -> logger::concepts::TupleLikeOfLogTargets<CharT>;
    };
;

/**
 * Provides a function to print a log messages.
 */
template <typename T, typename CharT>
concept PrintsToLog = requires(
    T t,
    std::basic_ostream<CharT> stream,
    const logger::LogContext<CharT>& context,
    std::basic_string_view<CharT> message) {
  { t.print(stream, context, message) };
};

/**
 * Meets the requirements of a log filter.
 */
template <typename T, typename CharT>
concept FiltersLog = requires(T t, const logger::LogContext<CharT>& context) {
  { t.filter(context) } noexcept -> std::same_as<bool>;
};

/**
 * Logger can write messages with a given context.
 */
template <typename T, typename Context>
concept WritableLogger =
    LogContextLike<Context> &&
    requires(
        T t,
        Context&& c,
        std::basic_string_view<logger::LogContextCharType<Context>> message) {
      t.write(std::forward<Context>(c), message);
    };

}  // namespace logger::concepts

#endif
