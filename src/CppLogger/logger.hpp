#pragma once

#include "concepts.hpp"

#include <format>
#include <iostream>
#include <iterator>
#include <print>
#include <ranges>
#include <source_location>
#include <string_view>
#include <syncstream>

/** Base namespace. */
namespace logger {

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

template <typename... Args> struct LogFormatString;

struct DefaultImplTag {};

template <typename> struct LogTargets;
using DefaultLogTargets = LogTargets<logger::DefaultImplTag>;

template <typename> struct LogFormatter;
using DefaultLogFormatter = LogFormatter<DefaultImplTag>;

template <typename> struct LogPrinter;
using DefaultLogPrinter = LogPrinter<DefaultImplTag>;

template <typename> struct LogFilter;
using DefaultLogFilter = LogFilter<DefaultImplTag>;

template <MessageType> struct MessageTypeTraits;

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
 * Injectable type which should satisfy
 * `logger::concepts::IndirectlyProvidesLogTargets`.
 *
 * Specialize for `DefaultProviderTag` to define custom behavior.
 */
template <typename T> struct LogTargets {

  /**
   * @return a range of target providers to be used for a given `MessageType`.
   *
   * Default implementation returns an empty view.
   */
  template <MessageType M> auto providers() const noexcept -> decltype(auto) {
    return std::ranges::empty_view<std::ostream &(*)()>{};
  }
};

/**
 * Base implementation of a log formatter that satisfies
 * `logger::concepts::IsLogFormatter`.
 *
 * By default, an object of this type with template argument
 * `DefaultFormatterTag` is used as the formatter type for logging. Custom
 * behavior can be provided by specializing that type.
 */
template <typename T> struct LogFormatter {

  /**
   * Formats a log message to an output iterator.
   *
   * By default, wraps `std::format_to`.
   *
   * @param[in] out iterator to the output buffer
   * @param[in] msg formatted log input message
   * @return iterator past the end of the output range
   */
  template <MessageType M, std::output_iterator<char> OutputIt>
  OutputIt
  format(const std::source_location & /**< [in] log message position */,
         OutputIt out, std::string_view msg) const noexcept {
    return std::format_to(std::move(out), "{}", msg);
  }
};

template <typename T> struct LogPrinter {
  template <MessageType MType, typename Stream>
    requires std::derived_from<Stream, std::remove_cvref_t<Stream>>
  void print(Stream &stream, const std::source_location &,
             std::string_view msg) const noexcept {
    std::print(stream, "{}", msg);
  }
}; // namespace logger

/**
 * Base implementation of a log filter that satisfies
 * `logger::concepts::IsLogFilter`.
 *
 * Specialize this type for template argument `DefaultLogFilterTag` to define
 * custom default filtering.
 */
template <typename T> struct LogFilter {

  /**
   * Determine if logging should be skipped for a particular `MessageType` and
   * `std::source_location`.
   *
   * Default implementation always return `true`.
   *
   * @return `true` if the message should be logged, `false` otherwise.
   */
  template <MessageType M>
  bool filter(const std::source_location
                  & /**< Source location of the log message */) const noexcept {
    return true;
  }
};

/**
 * Provides the default types used for logging a message of a given type.
 */
template <MessageType M> struct MessageTypeTraits {

  /** `MessageType` to which these traits belong */
  static constexpr MessageType type = M;

  /** Provider of log targets */
  using TargetProvider = DefaultLogTargets;

  /** Formatter of log messages */
  using Formatter = DefaultLogFormatter;

  using Printer = DefaultLogPrinter;

  /** Filter of log messages */
  using Filter = DefaultLogFilter;

  static_assert(concepts::IndirectlyProvidesLogTargets<TargetProvider, M>);
  static_assert(concepts::IsLogFormatter<Formatter, M>);
  static_assert(concepts::IsLogFilter<Filter, M>);
};

/**
 * Base logging implementation.
 */
template <
    MessageType MType,
    concepts::IndirectlyProvidesLogTargets<MType> Providers =
        MessageTypeTraits<MType>::TargetProvider,
    concepts::PrintsToLog<MType> Printer = MessageTypeTraits<MType>::Printer,
    concepts::IsLogFilter<MType> Filter = MessageTypeTraits<MType>::Filter,
    typename... Args>
void log(LogFormatString<std::type_identity_t<Args>...> fmt,
         Args &&...args) noexcept {
  if (!Filter{}.template filter<MType>(fmt.location()))
    return;
  std::string message =
      std::format(std::move(fmt), std::forward<Args>(args)...);
  Printer printer{};
  auto fns = [&message, location = fmt.location(),
              &printer]<concepts::ProvidesLogTarget... Ps>(Ps &&...ps) {
    (
        [&](auto &&s) {
          std::osyncstream sync{s};
          printer.template print<MType>(sync, location, message);
        }(ps()),
        ...);
  };
  using ProvidersType =
      decltype(std::declval<Providers>().template providers<MType>());
  if constexpr (concepts::TupleLikeLogTargetProviders<ProvidersType>) {
    std::apply(fns, Providers{}.template providers<MType>());
  } else if constexpr (concepts::LogTargetProviderRange<ProvidersType>) {
    for (auto p : Providers{}.template providers<MType>()) {
      fns(p());
    }
  }
}

/**
 * Log using defaults for `MessageType::Fatal`.
 *
 * @param[in] fmt format string input
 * @param[in] args variables to formated
 */
template <typename... Args>
void logFatal(LogFormatString<std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MessageType::Fatal>(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Error`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logError(LogFormatString<std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MessageType::Error>(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Warning`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logWarn(LogFormatString<std::type_identity_t<Args>...> fmt,
             Args &&...args) noexcept {
  log<MessageType::Warning>(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Info`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logInfo(LogFormatString<std::type_identity_t<Args>...> fmt,
             Args &&...args) noexcept {
  log<MessageType::Info>(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Debug`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logDebug(LogFormatString<std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MessageType::Debug>(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Verbose`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logVerbose(LogFormatString<std::type_identity_t<Args>...> fmt,
                Args &&...args) noexcept {
  log<MessageType::Verbose>(std::move(fmt), std::forward<Args>(args)...);
}

} // namespace logger
