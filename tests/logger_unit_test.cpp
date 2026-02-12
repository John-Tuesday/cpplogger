#include <cpplogger/context.hpp>
#include <cpplogger/logger.hpp>

#include <expected>
#include <iostream>
#include <print>
#include <sstream>

namespace cpplogger::test {

std::expected<void, std::string> testFormatTo();

std::expected<void, std::wstring> testWideFormatTo();

template <typename Context>
std::expected<void, std::string> testLog(Context&&);

template <typename Context>
std::expected<void, std::wstring> testWideLog(Context&&);

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
  cpplogger::LogContext context{std::source_location::current()};
  if (std::expected result = cpplogger::test::testLog(context); !result) {
    std::format_to(
        std::ostreambuf_iterator{std::wcerr},
        "Failed!\n{}\n",
        result.error());
    return 1;
  }
  if (std::expected result = cpplogger::test::testWideLog(context); !result) {
    std::format_to(
        std::ostreambuf_iterator{std::wcerr},
        L"Failed!\n{}\n",
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

template <typename Context>
std::expected<void, std::string> cpplogger::test::testLog(Context&& context) {
  cpplogger::Logger<char> logger{};
  std::stringstream expectStream{};
  logger.formatTo(
      std::ostreambuf_iterator{expectStream},
      context,
      "test log\n");
  std::string_view expect = expectStream.view();
  std::stringbuf capture{};
  auto* old = std::cerr.rdbuf(&capture);
  logger.log(context, "test log");
  std::cerr.rdbuf(old);
  std::string_view actual = capture.view();
  if (expect == actual) {
    return {};
  }
  return std::unexpected{std::format(
      "expect: '{}'\n"
      "actual: '{}'\n",
      expect,
      actual)};
}

template <typename Context>
std::expected<void, std::wstring>
cpplogger::test::testWideLog(Context&& context) {
  cpplogger::Logger<wchar_t> logger{};
  std::wstringstream expectStream{};
  logger.formatTo(
      std::ostreambuf_iterator{expectStream},
      context,
      L"test log\n");
  const std::wstring_view expect = expectStream.view();
  std::wstringbuf capture{};
  auto* old = std::wcerr.rdbuf(&capture);
  logger.log(context, L"test log");
  std::wcerr.rdbuf(old);
  std::wstring_view actual = capture.view();
  if (expect == actual) {
    return {};
  }
  return std::unexpected{std::format(
      L"expect: '{}'\n"
      L"actual: '{}'\n",
      expect,
      actual)};
}
