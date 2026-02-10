#include <cpplogger/logger.hpp>
#include <cpplogger/message.hpp>

#include <expected>
#include <iostream>
#include <print>
#include <sstream>

namespace cpplogger::test {

std::expected<void, std::string> testFormatTo();

std::expected<void, std::wstring> testWideFormatTo();

}  // namespace cpplogger::test

int main() {
  if (std::expected result = cpplogger::test::testFormatTo(); !result) {
    std::println(std::cerr, "Failed!\n{}", result.error());
    return 1;
  }
  if (std::expected result = cpplogger::test::testWideFormatTo(); !result) {
    std::format_to(
        std::ostreambuf_iterator{std::wcerr},
        L"Failed!\n{}",
        result.error());
    return 1;
  }
  return 0;
}

std::expected<void, std::string> cpplogger::test::testFormatTo() {
  constexpr cpplogger::LogContext context{};
  std::string expect =
      std::format("{}one two three equal 1 2 3!", context.file_name());
  std::stringstream capture{};
  cpplogger::Logger<char> logger{};
  logger.formatTo(
      std::ostreambuf_iterator{capture},
      context,
      "one two three equal {} {} {}!",
      1,
      2,
      3);
  if (expect == capture.view())
    return {};
  return std::unexpected{std::format(
      "expected: '{}'\n"
      "actual: '{}'",
      expect,
      capture.view())};
}

std::expected<void, std::wstring> cpplogger::test::testWideFormatTo() {
  constexpr cpplogger::LogContext context{std::source_location::current()};
  std::wstringstream expStream{};
  std::format_to(
      std::ostreambuf_iterator{expStream},
      "{}one two three equal 1 2 3!",
      context.file_name());
  std::wstring_view expect = expStream.view();
  std::wstringstream capture{};
  cpplogger::Logger<wchar_t> logger{};
  logger.formatTo(
      std::ostreambuf_iterator{capture},
      context,
      L"one two three equal {} {} {}!",
      1,
      2,
      3);
  if (expect == capture.view())
    return {};
  return std::unexpected{std::format(
      L"expected: '{}'\n"
      L"actual: '{}'",
      expect,
      capture.view())};
}
