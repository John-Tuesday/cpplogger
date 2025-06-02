#pragma once

#include "concepts.hpp"
#include "functors.hpp"
#include "message.hpp"

#include <format>
#include <syncstream>

/** Base namespace. */
namespace logger {

struct LoggerBase;
struct DefaultLogger;

template <MessageType MType, typename Logger>
void writeLog(Logger &&, std::string_view message,
              const std::source_location &location);

/**
 * Compile-time interface for writing logs.
 *
 * Provides default logging behavior which can be customize by satisfying
 * certain concepts in `logger::concepts`.
 */
struct LoggerBase {
  /**
   * Write logs to output targets.
   *
   * This is the function you should call if you want chain loggers together.
   *
   * Filtering is enforced if `Self` supports `logger::concepts::FiltersLog`.
   * Output targets are be provided by `Self` when it satifies
   * `logger::concepts::ProvidesLogOutputTargets`. Likewise, printing behavior
   * is be provided by satisfying `logger::concepts::PrintsToLog`.
   *
   * The intention is to not need to customize this function directly in order
   * to accomplish any custom behavior like formatting differently per target.
   *
   * @param[in] message log text common to all targets and printers.
   * @param[in] location describes the context of the log message.
   */
  template <MessageType MType, typename Self>
  void write(this Self &&self, std::string_view message,
             const std::source_location &location) {
    logger::writeLog<MType>(std::forward<Self>(self), message, location);
  }

  /**
   * Format and write a log message.
   *
   * Formats the input, then calls `write()` which where the core logic lies.
   *
   * @param[in] fmt format string.
   * @param[in] args values to be formatted
   */
  template <MessageType MType, typename Self, typename... Args>
  void log(this Self &&self, LogFormatString<std::type_identity_t<Args>...> fmt,
           Args &&...args) {
    self.template write<MType>(std::format(fmt, std::forward<Args>(args)...),
                               fmt.location());
  }
};

/**
 * Logging interface which default to implementations provided by
 * `logger::MessageTypeTraits`.
 */
struct DefaultLogger : public logger::LoggerBase {
  template <MessageType MType>
  auto targets(this auto &&self, const std::source_location &location) noexcept
      -> logger::concepts::TupleLikeOfLogTargets decltype(auto) {
    return (typename logger::MessageTypeTraits<MType>::Targets){}
        .template targets<MType>(location);
  }

  template <MessageType MType>
  bool filter(const std::source_location &location) const noexcept {
    return (typename logger::MessageTypeTraits<MType>::Filter){}
        .template filter<MType>(location);
  }

  template <MessageType MType, logger::concepts::LogTarget Stream>
  void print(this auto &&self, Stream &&stream,
             const std::source_location &location,
             std::string_view message) noexcept {
    return (typename logger::MessageTypeTraits<MType>::Printer){}
        .template print<MType>(std::forward<Stream>(stream), location, message);
  }
};

template <MessageType MType, typename Logger>
void writeLog(Logger &&logger, std::string_view message,
              const std::source_location &location) {
  if constexpr (logger::concepts::FiltersLog<decltype(logger), MType>) {
    if (!logger.template filter<MType>(location)) {
      return;
    }
  }
  if constexpr (logger::concepts::ProvidesLogOutputTargets<decltype(logger),
                                                           MType> &&
                logger::concepts::PrintsToLog<decltype(logger), MType>) {
    std::apply(
        [&logger, &location,
         &message]<logger::concepts::LogTarget... Ts>(Ts &&...ts) {
          (logger.template print<MType>(std::osyncstream{ts}, location,
                                        message),
           ...);
        },
        logger.template targets<MType>(location));
  }
}

template <MessageType MType, typename Logger = DefaultLogger, typename... Args>
  requires std::default_initializable<Logger>
void log(LogFormatString<std::type_identity_t<Args>...> fmt, Args &&...args) {
  std::source_location location = fmt.location();
  std::string message = std::format(fmt, std::forward<Args>(args)...);
  (Logger{}).template write<MType>(message, location);
  // writeLog<MType>( message, location);
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
