#include <CppLogger/logger.hpp>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <source_location>
#include <tuple>

template <> struct logger::LogTargets<logger::DefaultProviderTag> {
  template <logger::MessageType M>
  auto logproviders() const noexcept -> decltype(auto) {
    return std::tuple([]() -> std::ostream & { return std::clog; });
  }
};

static_assert(logger::concepts::IndirectlyProvidesLogTargets<
              logger::LogTargets<logger::DefaultProviderTag>,
              logger::MessageType::Warning>);

template <>
template <logger::MessageType M, std::output_iterator<char> OutputIt>
OutputIt logger::LogFormatter<logger::DefaultFormatterTag>::format(
    const std::source_location &location, OutputIt out,
    std::string_view msg) const noexcept {
  return std::format_to(std::move(out), "[{}] {} `{}` {}:{} {}", "VOID",
                        location.file_name(), location.function_name(),
                        location.line(), location.column(), msg);
}

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
  auto logproviders() const noexcept -> decltype(auto) {
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
  logger::logwarn("str fmt [{}]", 10);
}

void test::testFileLog() {
  constexpr auto mtype = logger::MessageType::Info;
  constexpr std::string_view expect = "info: 5 == 5";
  logger::logPrint<mtype, test::LogTargetsBasicFileLog>("info: 5 == {}", 5);
  std::ifstream logIn{test::LogTargetsBasicFileLog::logPath()};
  bool noLineRead{true};
  for (std::string line; std::getline(logIn, line, '\n');) {
    noLineRead = false;
    if (line != expect) {
      std::println("!!!FAIL!!!");
      std::println("  expect: '{}'", expect);
      std::println("  actual: '{}'", line);
    } else {
      std::println("PASS");
    }
  }
  assert(!noLineRead);
}
