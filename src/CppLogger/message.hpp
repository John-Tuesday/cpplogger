#pragma once

#include <format>
#include <source_location>

namespace logger {

enum class MessageType;
template <typename...> struct LogFormatString;

/**
 * Base interface for logging context.
 *
 * Essentially a wrapper around `std::source_location`.
 *
 * This type may be subclassed to add custom functionality to context's given to
 * various logger functions. Simply provide the type when invoking a log
 * function. Consumers can then distinguish different context types by way of
 * function overloads or template specialization.
 *
 * Custom logging context classes must be constructible from
 * `std::source_location` when used as the template argument of a call to log.
 */
struct LogContext : public std::source_location {};

/**
 * LogContext with basic severity levels.
 */
template <MessageType MType> struct MTypeContext : public LogContext {};

/** Fundamental log message types. */
enum class MessageType {
  /** Most severe. */
  Fatal,
  /** Less severe than `Fatal`. */
  Error,
  /** Less severe than `Error`. */
  Warning,
  /** Less severe than `Warning`. */
  Info,
  /** Less severe than `Info`. */
  Debug,
  /** Less severe than `Debug`. */
  Verbose,
};

/** Format string with extra information useful when logging. */
template <typename... Args>
struct LogFormatString
    : public std::format_string<std::type_identity_t<Args>...> {

  /**
   * Forwards `fmt` to `std::format_string`.
   *
   * @note Generally, no input should be given for `location`; this way the
   * `std::source_location` will be constructed with respect to the callsite.
   *
   * @param[in] fmt forwarded as the first argument of the constructor for
   * `std::format_string<>`
   * @param[in] location location information to associated with this log
   * message
   */
  template <typename T>
  consteval LogFormatString(
      T &&fmt, std::source_location location = std::source_location::current())
      : std::format_string<std::type_identity_t<Args>...>(std::forward<T>(fmt)),
        m_location(location) {}

  /**
   * Source location information associated with the format string.
   *
   * @return associated location
   */
  constexpr std::source_location location() const noexcept {
    return m_location;
  }

private:
  std::source_location m_location{};
};

} // namespace logger
