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

template <typename Logger, concepts::LogContextLike Context>
void writeLog(Logger &&logger, Context &&context,
              std::basic_string_view<LogContextCharType<Context>> message);

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
  template <concepts::ConstructibleLogContextLike Context, typename Self,
            typename... Args>
  void log(this Self &&self,
           logger::LogFormatString<logger::LogContextCharType<Context>,
                                   std::type_identity_t<Args>...>
               fmt,
           Args &&...args) {
    self.write(Context{logger::LogContext<logger::LogContextCharType<Context>>{
                   fmt.location()}},
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
    requires std::same_as<LogContextCharType<Context>, char>
  auto targets(const Context &) noexcept
      -> logger::concepts::TupleLikeOfLogTargets<
          LogContextCharType<Context>> decltype(auto) {
    return std::tuple(std::ref(std::cerr));
  }

  template <concepts::LogContextLike Context>
    requires std::same_as<LogContextCharType<Context>, wchar_t>
  auto targets(const Context &) noexcept
      -> logger::concepts::TupleLikeOfLogTargets<
          LogContextCharType<Context>> decltype(auto) {
    return std::tuple(std::ref(std::wcerr));
  }

  template <concepts::LogContextLike Context>
  bool filter(const Context &) const noexcept {
    return true;
  }

  template <
      concepts::LogContextLike Context,
      logger::concepts::PrintableStream<LogContextCharType<Context>> Stream>
    requires std::same_as<LogContextCharType<Context>, char>
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
template <concepts::ConstructibleLogContextLike Context,
          concepts::WritableLogger<Context> Logger =
              logger::LoggerDefaults<logger::DefaultImplTag>,
          typename CharT = logger::LogContextCharType<Context>,
          typename... Args>
  requires std::default_initializable<Logger>
void log(LogFormatString<CharT, std::type_identity_t<Args>...> fmt,
         Args &&...args) {
  Logger{}.write(
      Context{logger::LogContext<logger::LogContextCharType<Context>>{
          fmt.location()}},
      std::format(fmt, std::forward<Args>(args)...));
}

/**
 * Log using defaults for `MessageType::Fatal`.
 *
 * @param[in] fmt format string input
 * @param[in] args variables to formated
 */
template <typename... Args>
void logFatal(LogFormatString<char, std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MTypeContext<MessageType::Fatal, char>>(std::move(fmt),
                                              std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Error`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logError(LogFormatString<char, std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MTypeContext<MessageType::Error, char>>(std::move(fmt),
                                              std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Warning`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logWarn(LogFormatString<char, std::type_identity_t<Args>...> fmt,
             Args &&...args) noexcept {
  log<MTypeContext<MessageType::Warning, char>>(std::move(fmt),
                                                std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Info`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logInfo(LogFormatString<char, std::type_identity_t<Args>...> fmt,
             Args &&...args) noexcept {
  log<MTypeContext<MessageType::Info, char>>(std::move(fmt),
                                             std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Debug`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logDebug(LogFormatString<char, std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MTypeContext<MessageType::Debug, char>>(std::move(fmt),
                                              std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Verbose`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logVerbose(LogFormatString<char, std::type_identity_t<Args>...> fmt,
                Args &&...args) noexcept {
  log<MTypeContext<MessageType::Verbose, char>>(std::move(fmt),
                                                std::forward<Args>(args)...);
}

} // namespace logger
