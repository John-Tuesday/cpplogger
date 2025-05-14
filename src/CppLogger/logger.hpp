#pragma once

#include "concepts.hpp"
#include "message.hpp"

#include <format>
#include <iostream>
#include <print>
#include <ranges>
#include <source_location>
#include <string_view>
#include <syncstream>

/** Base namespace. */
namespace logger {

/**
 * Tag used to enable custom specialization of default types
 */
struct DefaultImplTag {};

template <typename> struct LogTargets;
using DefaultLogTargets = LogTargets<logger::DefaultImplTag>;

template <typename> struct LogPrinter;
using DefaultLogPrinter = LogPrinter<DefaultImplTag>;

template <typename> struct LogFilter;
using DefaultLogFilter = LogFilter<DefaultImplTag>;

template <MessageType> struct MessageTypeTraits;

/**
 * Injectable type which should satisfy
 * `logger::concepts::IndirectlyProvidesLogTargets`.
 *
 * Specialize for `DefaultImplTag` to define custom behavior.
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
 * Base implementation of a log printer.
 */
template <typename T> struct LogPrinter {
  /**
   * Print log message to a given log target.
   *
   * This function is responsible for formatting the log message, including
   * appending a newline.
   *
   * Specialize different `Stream` to provide different behavior for different
   * stream types. For example, different behavior for `std::ofstream` than
   * `std::ostream`.
   *
   * @tparam MType log message type
   * @tparam Stream log target stream
   * @param stream log output stream which will be written to
   * @param location information describing the original callsite
   * @param msg message specified by the original callsite
   */
  template <MessageType MType, typename Stream>
    requires std::derived_from<Stream, std::remove_cvref_t<Stream>>
  void print(Stream &stream, const std::source_location &location,
             std::string_view msg) const noexcept {
    std::println(stream, "{} {}:{}", location.file_name(), location.line(),
                 location.column(), msg);
  }
};

/**
 * Base implementation of a log filter that satisfies
 * `logger::concepts::IsLogFilter`.
 *
 * Specialize this type for template argument `DefaultImplTag` to define
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
template <MessageType MType> struct MessageTypeTraits {
  /** Provider of log targets */
  using TargetProvider = DefaultLogTargets;
  static_assert(concepts::IndirectlyProvidesLogTargets<TargetProvider, MType>);

  /** Printer for log messages */
  using Printer = DefaultLogPrinter;
  static_assert(concepts::PrintsToLog<Printer, MType>);

  /** Filter of log messages */
  using Filter = DefaultLogFilter;
  static_assert(concepts::FiltersLog<Filter, MType>);
};

/**
 * Base logging implementation.
 */
template <
    MessageType MType,
    concepts::IndirectlyProvidesLogTargets<MType> Providers =
        MessageTypeTraits<MType>::TargetProvider,
    concepts::PrintsToLog<MType> Printer = MessageTypeTraits<MType>::Printer,
    concepts::FiltersLog<MType> Filter = MessageTypeTraits<MType>::Filter,
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
