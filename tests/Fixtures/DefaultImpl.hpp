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
