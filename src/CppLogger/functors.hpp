#pragma once

#include "concepts.hpp"
#include "message.hpp"

#include <iostream>
#include <print>
#include <ranges>
#include <source_location>
#include <string_view>

namespace logger {

/**
 * Tag used to enable custom specialization of default types
 */
struct DefaultImplTag {};

template <typename> struct LogTargets;
using DefaultLogTargets = LogTargets<logger::DefaultImplTag>;

template <typename> struct LogTargetProviders;
using DefaultLogTargetProviders = LogTargetProviders<logger::DefaultImplTag>;

template <typename> struct LogPrinter;
using DefaultLogPrinter = LogPrinter<DefaultImplTag>;

template <typename> struct LogFilter;
using DefaultLogFilter = LogFilter<DefaultImplTag>;

template <MessageType> struct MessageTypeTraits;

template <typename> struct LogTargets {
  template <MessageType>
  auto targets(const std::source_location &) noexcept
      -> logger::concepts::TupleLikeOfLogTargets decltype(auto) {
    return std::tuple(std::ref(std::cerr));
  }
};

/**
 * Injectable type which should satisfy
 * `logger::concepts::IndirectlyProvidesLogTargets`.
 *
 * Specialize for `DefaultImplTag` to define custom behavior.
 */
template <typename T> struct LogTargetProviders {

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
  template <MessageType MType, logger::concepts::LogTarget Stream>
  void print(Stream &&stream, const std::source_location &location,
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
