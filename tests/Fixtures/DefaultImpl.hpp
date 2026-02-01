#pragma once

#include "tempfiles.hpp"

#include <cpplogger/logger.hpp>

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
