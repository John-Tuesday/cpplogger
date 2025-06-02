#pragma once

#include "CppLogger/logger.hpp"

namespace logger::test {

struct DoubleCerrLogger : public logger::LoggerDefaults<void> {
  template <logger::MessageType>
  auto targets(const std::source_location &) noexcept
      -> logger::concepts::TupleLikeOfLogTargets decltype(auto) {
    return std::tuple(std::ref(std::cerr), std::ref(std::cerr));
  }
};

template <typename... Ts> struct ChainLogger : public LoggerBase {
  template <concepts::LogContextFrom Context>
  void write(Context &&context, std::string_view message) {
    (Ts{}.write(context, message), ...);
  }
};

} // namespace logger::test

namespace logger {

template <typename T> struct LogHelpers {
  auto targets(const logger::LogContext &context) noexcept
      -> logger::concepts::TupleLikeOfLogTargets decltype(auto) {
    std::println("TARGETS: LogCtxt");
    return std::tuple(std::ref(std::cerr));
  }

  bool filter(const logger::LogContext &context) const noexcept {
    std::println("Filter: Log");
    return true;
  }

  template <logger::concepts::PrintableStream Stream>
  void print(Stream &&stream, const logger::LogContext &context,
             std::string_view message) {
    std::println(stream, "LogContext: {}", message);
  }
};

struct TemplHelpers {
  template <concepts::LogContextFrom Context>
  auto targets(Context &&context) noexcept
      -> logger::concepts::TupleLikeOfLogTargets decltype(auto) {
    std::println("TARGETS: TemplCtx");
    return std::tuple(std::ref(std::cerr));
  }

  template <concepts::LogContextFrom Context>
  bool filter(const Context &context) const noexcept {
    std::println("FILTER: TemplCtx: TEMPLATE < >");
    return true;
  }

  bool filter(const logger::LogContext &context) const noexcept {
    std::println("FILTER: TemplCtx: OVERLOAD");
    return true;
  }

  template <concepts::LogContextFrom Context,
            logger::concepts::PrintableStream Stream>
  void print(Stream &&stream, Context &&context, std::string_view message) {
    std::println(stream, "TemplCtx: {}", message);
  }
};

struct DefaultLogger_ : public LoggerBase, public LogHelpers<void> {};

struct TemplLogger : public LoggerBase, public TemplHelpers {};

} // namespace logger
