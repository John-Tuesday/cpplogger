#pragma once

#include "tempfiles.hpp"

#include "CppLogger/logger.hpp"

#include <fstream>
#include <tuple>

namespace logger::test {

struct DoubleCerrLogger : public logger::LoggerDefaults<void> {
  template <logger::concepts::LogContextFrom<char> Context>
  auto targets(const Context &) noexcept
      -> logger::concepts::TupleLikeOfLogTargets<
          LogContextCharType<Context>> decltype(auto) {
    return std::tuple(std::ref(std::cerr), std::ref(std::cerr));
  }
};

template <typename... Ts> struct ChainLogger : public LoggerBase {
  template <concepts::LogContextLike Context>
  void write(Context &&context,
             std::basic_string_view<LogContextCharType<Context>> message) {
    (Ts{}.write(context, message), ...);
  }
};

struct LogTargetsBasicFileLog : public logger::LoggerDefaults<void> {

  template <logger::concepts::LogContextLike Context>
  auto targets(const Context &location) const noexcept
      -> logger::concepts::TupleLikeOfLogTargets<
          LogContextCharType<Context>> decltype(auto) {
    return std::tuple(std::ref(std::clog), std::ofstream{logPath()});
  }

  static std::filesystem::path logPath();
};

inline std::filesystem::path LogTargetsBasicFileLog::logPath() {
  std::filesystem::path path = test::tempDir();
  std::error_code ec{};
  std::filesystem::create_directory(path, ec);
  assert(!ec);
  return path / "test.log";
}

} // namespace logger::test

namespace logger {

template <typename T> struct LogHelpers {
  template <typename CharT>
  auto targets(const logger::LogContext<CharT> &context) noexcept
      -> logger::concepts::TupleLikeOfLogTargets<CharT> decltype(auto) {
    std::println("TARGETS: LogCtxt");
    return std::tuple(std::ref(std::cerr));
  }

  template <typename CharT>
  bool filter(const logger::LogContext<CharT> &context) const noexcept {
    std::println("Filter: Log");
    return true;
  }

  template <typename Stream, typename CharT>
    requires logger::concepts::PrintableStream<Stream, CharT>
  void print(Stream &&stream, const logger::LogContext<CharT> &context,
             std::basic_string_view<CharT> message) {
    std::println(stream, "LogContext: {}", message);
  }
};

struct TemplHelpers {
  template <concepts::LogContextLike Context>
    requires std::same_as<logger::LogContextCharType<Context>, char>
  auto targets(Context &&context) noexcept
      -> logger::concepts::TupleLikeOfLogTargets<
          LogContextCharType<Context>> decltype(auto) {
    std::println("TARGETS: TemplCtx");
    return std::tuple(std::ref(std::cerr));
  }

  template <concepts::LogContextLike Context>
    requires std::same_as<logger::LogContextCharType<Context>, char>
  bool filter(const Context &context) const noexcept {
    std::println("FILTER: TemplCtx: TEMPLATE < >");
    return true;
  }

  bool filter(const logger::LogContext<char> &context) const noexcept {
    std::println("FILTER: TemplCtx: OVERLOAD");
    return true;
  }

  template <
      concepts::LogContextLike Context,
      logger::concepts::PrintableStream<LogContextCharType<Context>> Stream>
  void print(Stream &&stream, Context &&context,
             std::basic_string_view<LogContextCharType<Context>> message) {
    std::println(stream, "TemplCtx: {}", message);
  }
};

struct DefaultLogger_ : public LoggerBase, public LogHelpers<void> {};

struct TemplLogger : public LoggerBase, public TemplHelpers {};

} // namespace logger
