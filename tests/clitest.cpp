#include "CppLogger/concepts.hpp"
#include <CppLogger/logger.hpp>

#include "Fixtures/tempfiles.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <tuple>

namespace test {

struct LogTargetsBasicFileLog : public logger::LoggerDefaults<void> {

  template <logger::concepts::LogContextFrom Context>
  auto targets(const Context &location) const noexcept
      -> logger::concepts::TupleLikeOfLogTargets decltype(auto) {
    return std::tuple(std::ref(std::clog), std::ref(m_logfile));
  }

  static std::filesystem::path logPath();

private:
  mutable std::ofstream m_logfile{logPath()};
};

std::filesystem::path LogTargetsBasicFileLog::logPath() {
  std::filesystem::path path = test::tempDir();
  std::error_code ec{};
  std::filesystem::create_directory(path, ec);
  assert(!ec);
  return path / "test.log";
}

void testBasic();

void testFileLog();

} // namespace test

int main() {
  std::println("Start main()\n");
  test::testBasic();
  test::testFileLog();
  std::println("\nend main()\n");
  return 0;
}

void test::testBasic() {
  constexpr std::string_view expect = "str fmt[10]";
  logger::logWarn("str fmt [{}]", 10);
}

void test::testFileLog() {
  constexpr auto mtype = logger::MessageType::Info;
  constexpr std::string_view expect = "info: 5 == 5";
  logger::log<logger::MTypeContext<mtype>, test::LogTargetsBasicFileLog>(
      "info: 5 == {}", 5);
  std::ifstream logIn{test::LogTargetsBasicFileLog::logPath()};
  bool noLineRead{true};
  for (std::string line; std::getline(logIn, line, '\n');) {
    noLineRead = false;
    if (line.contains(expect)) {
      std::println("!!!FAIL!!!");
      std::println("  expect: '{}'", expect);
      std::println("  actual: '{}'", line);
    } else {
      std::println("PASS");
    }
  }
  assert(!noLineRead);
}
