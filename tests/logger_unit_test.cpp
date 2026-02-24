#include <cpplogger/LogContextFormatter.hpp>
#include <cpplogger/context.hpp>
#include <cpplogger/logger.hpp>

#include <expected>
#include <iostream>
#include <print>
#include <sstream>

namespace cpplogger::test {

std::string errorMessage(
    std::string_view expect,
    std::string_view actual,
    std::source_location location = std::source_location::current()) {
  return std::format(
      "in function: {}\n"
      "expect: '{}'\n"
      "actual: '{}'",
      location.function_name(),
      expect,
      actual);
}

std::wstring errorMessage(
    std::wstring_view expect,
    std::wstring_view actual,
    std::source_location location = std::source_location::current()) {
  std::wstringstream stream;
  std::ostreambuf_iterator out{stream};
  out = std::format_to(out, "in function: {}\n", location.function_name());
  std::format_to(
      out,
      L"expect: '{}'\n"
      L"actual: '{}'",
      expect,
      actual);
  return std::move(stream).str();
}

std::expected<void, std::string> testFormatTo(
    cpplogger::BasicLogContext context = {std::source_location::current()});

std::expected<void, std::wstring> testWideFormatTo(
    cpplogger::BasicLogContext context = {std::source_location::current()});

template <typename Context>
std::expected<void, std::string> testLog(Context&&);

template <typename Context>
std::expected<void, std::wstring> testWideLog(Context&&);

}  // namespace cpplogger::test

int main() {
  if (std::expected result = cpplogger::test::testFormatTo(); !result) {
    std::println(std::cerr, "{}", result.error());
    return 1;
  }
  if (std::expected result = cpplogger::test::testWideFormatTo(); !result) {
    std::wcerr << result.error() << "\n";
    return 1;
  }
  cpplogger::BasicLogContext context{std::source_location::current()};
  if (std::expected result = cpplogger::test::testLog(context); !result) {
    std::println(std::cerr, "{}\n", result.error());
    return 1;
  }
  if (std::expected result = cpplogger::test::testWideLog(context); !result) {
    std::wcerr << result.error() << "\n";
    return 1;
  }
  return 0;
}

std::expected<void, std::string>
cpplogger::test::testFormatTo(cpplogger::BasicLogContext context) {
  std::string expect = std::format("{} one two three equal 1 2 3!", context);
  std::stringstream capture{};
  cpplogger::BasicLogger<char> logger{};
  logger.formatTo(
      std::ostreambuf_iterator{capture},
      context,
      "one two three equal {} {} {}!",
      1,
      2,
      3);
  if (expect != capture.view())
    return std::unexpected{
        cpplogger::test::errorMessage(expect, capture.view())};
  return {};
}

std::expected<void, std::wstring>
cpplogger::test::testWideFormatTo(cpplogger::BasicLogContext context) {
  std::wstringstream expectStream;
  std::format_to(
      std::ostreambuf_iterator{expectStream},
      "{} one two three equal 1 2 3!",
      context);
  std::wstring_view expect = expectStream.view();
  std::wstringstream capture{};
  cpplogger::BasicLogger<wchar_t> logger{};
  logger.formatTo(
      std::ostreambuf_iterator{capture},
      context,
      L"one two three equal {} {} {}!",
      1,
      2,
      3);
  if (expect != capture.view())
    return std::unexpected{
        cpplogger::test::errorMessage(expect, capture.view())};
  return {};
}

template <typename Context>
std::expected<void, std::string> cpplogger::test::testLog(Context&& context) {
  cpplogger::BasicLogger<char> logger{};
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
  if (expect != actual)
    return std::unexpected{cpplogger::test::errorMessage(expect, actual)};
  return {};
}

template <typename Context>
std::expected<void, std::wstring>
cpplogger::test::testWideLog(Context&& context) {
  cpplogger::BasicLogger<wchar_t> logger{};
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
  if (expect != actual)
    return std::unexpected{cpplogger::test::errorMessage(expect, actual)};
  return {};
}
