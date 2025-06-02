#pragma once

#include "functors.hpp"

#include <format>
#include <source_location>

namespace logger {

enum class MessageType;
template <MessageType> struct MessageTypeTraits;
template <typename...> struct LogFormatString;

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

/**
 * Provides the default types used for logging a message of a given type.
 */
template <MessageType MType> struct MessageTypeTraits {
  using Targets = DefaultLogTargets;
  static_assert(concepts::ProvidesLogOutputTargets<Targets, MType>);

  /** Provider of log targets */
  using TargetProvider = DefaultLogTargetProviders;
  static_assert(concepts::ProvidesIndirectLogTargets<TargetProvider, MType>);

  /** Printer for log messages */
  using Printer = DefaultLogPrinter;
  static_assert(concepts::PrintsToLog<Printer, MType>);

  /** Filter of log messages */
  using Filter = DefaultLogFilter;
  static_assert(concepts::FiltersLog<Filter, MType>);
};

} // namespace logger
