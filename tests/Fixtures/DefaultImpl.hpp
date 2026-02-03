#pragma once

#ifndef TESTS_FIXTURES_DEFAULTIMPL_HPP
#define TESTS_FIXTURES_DEFAULTIMPL_HPP

#include <cpplogger/logger.hpp>

#include <tuple>

namespace logger::test {

struct DoubleCerrLogger : public logger::LoggerDefaults<void> {
  template <logger::concepts::LogContextFrom<char> Context>
  auto targets(const Context&) noexcept
      -> logger::concepts::TupleLikeOfLogTargets<
          LogContextCharType<Context>> decltype(auto) {
    return std::tuple(std::ref(std::cerr), std::ref(std::cerr));
  }
};

template <typename... Ts>
struct ChainLogger : public LoggerBase {
  template <concepts::LogContextLike Context>
  void write(
      Context&& context,
      std::basic_string_view<LogContextCharType<Context>> message) {
    (Ts{}.write(context, message), ...);
  }
};

}  // namespace logger::test

#endif
