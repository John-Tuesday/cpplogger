#include <CppLogger/logger.hpp>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <tuple>

template <> struct logger::LogTargetProviders<logger::DefaultImplTag> {
  template <logger::MessageType M>
  auto providers() const noexcept -> decltype(auto) {
    return std::tuple([]() -> std::ostream & { return std::clog; });
  }
};

static_assert(logger::concepts::ProvidesIndirectLogTargets<
              logger::DefaultLogTargetProviders, logger::MessageType::Warning>);

namespace test {

std::filesystem::path tempDir() {
  std::error_code ec{};
  std::filesystem::path path =
      std::filesystem::temp_directory_path(ec) / "CppLogger";
  if (ec) {
    std::println(std::clog, "Error creating temp dir: '{}'", ec.message());
    assert(!ec);
    return std::filesystem::path{};
  }
  assert(!ec);
  return path;
}

struct LogTargetsBasicFileLog {
  template <logger::MessageType MType>
  auto providers() const noexcept -> decltype(auto) {
    return std::tuple([]() -> std::ostream & { return std::clog; },
                      [this]() { return std::ref(this->m_logfile); });
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
  logger::log<mtype, test::LogTargetsBasicFileLog>("info: 5 == {}", 5);
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
