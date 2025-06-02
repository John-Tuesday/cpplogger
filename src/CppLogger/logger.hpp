#pragma once

#include "concepts.hpp"
#include "message.hpp"

#include <format>
#include <print>
#include <syncstream>

/** Base namespace. */
namespace logger {

struct DefaultImplTag;

struct LoggerBase;
template <typename> struct LoggerDefaults;

template <concepts::ConstructibleLogContext Context,
          typename Logger = LoggerDefaults<logger::DefaultImplTag>,
          typename... Args>
  requires std::default_initializable<Logger>
void log(LogFormatString<std::type_identity_t<Args>...> fmt, Args &&...args);

template <typename Logger, concepts::LogContextFrom Context>
void writeLog(Logger &&logger, Context &&context, std::string_view message);

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
   */
  template <concepts::LogContextFrom Context, typename Self>
  void write(this Self &&self, Context &&context, std::string_view message) {
    logger::writeLog(std::forward<Self>(self), std::forward<Context>(context),
                     message);
  }

  /**
   * Format and write a log message.
   *
   * Formats the input, then calls `write()` which where the core logic lies.
   *
   * @param[in] fmt format string.
   * @param[in] args values to be formatted
   */
  template <concepts::ConstructibleLogContext Context = logger::LogContext,
            typename Self, typename... Args>
  void log(this Self &&self,
           logger::LogFormatString<std::type_identity_t<Args>...> fmt,
           Args &&...args) {
    self.write(Context{fmt.location()},
               std::format(fmt, std::forward<Args>(args)...));
  }
};

template <typename> struct LoggerDefaults : public LoggerBase {
  template <concepts::LogContextFrom Context>
  auto targets(const Context &) noexcept
      -> logger::concepts::TupleLikeOfLogTargets decltype(auto) {
    return std::tuple(std::ref(std::cerr));
  }

  template <concepts::LogContextFrom Context>
  bool filter(const Context &) const noexcept {
    return true;
  }

  template <concepts::LogContextFrom Context,
            logger::concepts::LogTarget Stream>
  void print(Stream &&stream, const Context &context,
             std::string_view msg) const noexcept {
    std::println(stream, "{} {}:{}", context.file_name(), context.line(),
                 context.column(), msg);
  }
};

template <typename Logger, concepts::LogContextFrom Context>
void writeLog(Logger &&logger, Context &&context, std::string_view message) {
  if constexpr (concepts::FiltersLog<decltype(logger)>) {
    if (!logger.filter(context)) {
      return;
    }
  }
  if constexpr (concepts::ProvidesLogOutputTargets<decltype(logger)> &&
                concepts::PrintsToLog<decltype(logger)>) {
    std::apply(
        [&logger, &context,
         &message]<logger::concepts::LogTarget... Ts>(Ts &&...ts) {
          (logger.print(std::osyncstream{ts}, context, message), ...);
        },
        logger.targets(context));
  }
}

template <concepts::ConstructibleLogContext Context, typename Logger,
          typename... Args>
  requires std::default_initializable<Logger>
void log(LogFormatString<std::type_identity_t<Args>...> fmt, Args &&...args) {
  Logger{}.write(Context{fmt.location()},
                 std::format(fmt, std::forward<Args>(args)...));
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
  log<MTypeContext<MessageType::Fatal>>(std::move(fmt),
                                        std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Error`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logError(LogFormatString<std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MTypeContext<MessageType::Error>>(std::move(fmt),
                                        std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Warning`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logWarn(LogFormatString<std::type_identity_t<Args>...> fmt,
             Args &&...args) noexcept {
  log<MTypeContext<MessageType::Warning>>(std::move(fmt),
                                          std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Info`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logInfo(LogFormatString<std::type_identity_t<Args>...> fmt,
             Args &&...args) noexcept {
  log<MTypeContext<MessageType::Info>>(std::move(fmt),
                                       std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Debug`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logDebug(LogFormatString<std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MTypeContext<MessageType::Debug>>(std::move(fmt),
                                        std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Verbose`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logVerbose(LogFormatString<std::type_identity_t<Args>...> fmt,
                Args &&...args) noexcept {
  log<MTypeContext<MessageType::Verbose>>(std::move(fmt),
                                          std::forward<Args>(args)...);
}

} // namespace logger
