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

template <typename Context, typename Logger, typename CharT, typename... Args>
  requires std::default_initializable<Logger> &&
           concepts::ConstructibleLogContext<Context, CharT>
void log(LogFormatString<CharT, std::type_identity_t<Args>...> fmt,
         Args &&...args);

template <typename Context, typename Logger, typename CharT, typename... Args>
  requires std::default_initializable<Logger> &&
           concepts::ConstructibleLogContext<Context, CharT>
void writeLog(Logger &&logger, Context &&context,
              std::basic_string_view<CharT> message);

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
  template <concepts::LogContextLike Context, typename Self>
  void write(this Self &&self, Context &&context,
             std::basic_string_view<LogContextCharType<Context>> message) {
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
  template <typename Context, typename CharT, typename Self, typename... Args>
    requires concepts::ConstructibleLogContext<Context, CharT>
  void log(this Self &&self,
           logger::LogFormatString<CharT, std::type_identity_t<Args>...> fmt,
           Args &&...args) {
    self.write(Context{fmt.location()},
               std::format(fmt, std::forward<Args>(args)...));
  }
};

/**
 * Logger with default implementations of the three helper functions.
 *
 * The default logger is `LoggerDefaults<logger::DefaultImplTag>`, but it may be
 * specialized to change the default logger.
 */
template <typename> struct LoggerDefaults : public LoggerBase {
  template <concepts::LogContextLike Context>
  auto targets(const Context &) noexcept
      -> logger::concepts::TupleLikeOfLogTargets<
          LogContextCharType<Context>> decltype(auto) {
    return std::tuple(std::ref(std::cerr));
  }

  template <concepts::LogContextLike Context>
  bool filter(const Context &) const noexcept {
    return true;
  }

  template <
      concepts::LogContextLike Context,
      logger::concepts::PrintableStream<LogContextCharType<Context>> Stream>
  void print(
      Stream &&stream, const Context &context,
      std::basic_string_view<LogContextCharType<Context>> msg) const noexcept {
    std::println(stream, "{} {}:{}", context.file_name(), context.line(),
                 context.column(), msg);
  }
};

/**
 * Provides the core logic for managing and writing logs.
 */
template <typename Logger, concepts::LogContextLike Context>
void writeLog(Logger &&logger, Context &&context,
              std::basic_string_view<LogContextCharType<Context>> message) {
  using CharT = LogContextCharType<Context>;
  if constexpr (concepts::FiltersLog<decltype(logger), CharT>) {
    if (!logger.filter(context)) {
      return;
    }
  }
  if constexpr (concepts::ProvidesLogOutputTargets<decltype(logger), CharT> &&
                concepts::PrintsToLog<decltype(logger), CharT>) {
    std::apply(
        [&logger, &context,
         &message]<logger::concepts::LogTarget<CharT>... Ts>(Ts &&...ts) {
          (logger.print(std::basic_osyncstream<CharT>{ts}, context, message),
           ...);
        },
        logger.targets(context));
  }
}

/**
 * Create a formatted message and use a default constructed `Logger` to log it.
 */
template <typename Context, typename Logger, typename CharT, typename... Args>
  requires std::default_initializable<Logger> &&
           concepts::ConstructibleLogContext<Context, CharT>
void log(LogFormatString<CharT, std::type_identity_t<Args>...> fmt,
         Args &&...args) {
  Logger{}.write(Context{logger::LogContext<CharT>{fmt.location()}},
                 std::format(fmt, std::forward<Args>(args)...));
}

/**
 * Log using defaults for `MessageType::Fatal`.
 *
 * @param[in] fmt format string input
 * @param[in] args variables to formated
 */
template <typename CharT, typename... Args>
void logFatal(LogFormatString<CharT, std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MTypeContext<MessageType::Fatal, CharT>>(std::move(fmt),
                                               std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Error`.
 *
 * @copydetails logFatal()
 */
template <typename CharT, typename... Args>
void logError(LogFormatString<CharT, std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MTypeContext<MessageType::Error, CharT>>(std::move(fmt),
                                               std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Warning`.
 *
 * @copydetails logFatal()
 */
template <typename CharT, typename... Args>
void logWarn(LogFormatString<CharT, std::type_identity_t<Args>...> fmt,
             Args &&...args) noexcept {
  log<MTypeContext<MessageType::Warning, CharT>>(std::move(fmt),
                                                 std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Info`.
 *
 * @copydetails logFatal()
 */
template <typename CharT, typename... Args>
void logInfo(LogFormatString<CharT, std::type_identity_t<Args>...> fmt,
             Args &&...args) noexcept {
  log<MTypeContext<MessageType::Info, CharT>>(std::move(fmt),
                                              std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Debug`.
 *
 * @copydetails logFatal()
 */
template <typename CharT, typename... Args>
void logDebug(LogFormatString<CharT, std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MTypeContext<MessageType::Debug, CharT>>(std::move(fmt),
                                               std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Verbose`.
 *
 * @copydetails logFatal()
 */
template <typename CharT, typename... Args>
void logVerbose(LogFormatString<CharT, std::type_identity_t<Args>...> fmt,
                Args &&...args) noexcept {
  log<MTypeContext<MessageType::Verbose, CharT>>(std::move(fmt),
                                                 std::forward<Args>(args)...);
}

} // namespace logger
