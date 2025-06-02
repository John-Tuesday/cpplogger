#pragma once

#include "CppLogger/logger.hpp"

namespace logger::test {

struct DoubleCerrLogger : public DefaultLogger {
  template <logger::MessageType>
  auto targets(const std::source_location &) noexcept
      -> logger::concepts::TupleLikeOfLogTargets decltype(auto) {
    return std::tuple(std::ref(std::cerr), std::ref(std::cerr));
  }
};

template <typename... Ts> struct ChainLogger : public LoggerBase {
  template <MessageType MType, typename... Args>
  void log(LogFormatString<std::type_identity_t<Args>...> fmt, Args &&...args) {
    std::string message = std::format(fmt, std::forward<Args>(args)...);
    (Ts{}.template write<MType>(message, fmt.location()), ...);
  }
};

} // namespace logger::test
