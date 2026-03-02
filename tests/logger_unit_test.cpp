#include <cpplogger/BasicLogger.hpp>
#include <cpplogger/Defaults.hpp>
#include <cpplogger/LogContextFormatter.hpp>
#include <cpplogger/context.hpp>

#include "tests/fixtures/result.hpp"

#include <expected>
#include <iostream>
#include <sstream>

namespace cpplogger::test {

cpplogger::test::Result testFormatTo(
    cpplogger::BasicLogContext context = {std::source_location::current()});

cpplogger::test::WResult testWideFormatTo(
    cpplogger::BasicLogContext context = {std::source_location::current()});

template <typename Context>
cpplogger::test::Result testFormat(Context&&);

template <typename Context>
cpplogger::test::WResult testWideFormat(Context&&);

template <typename Context>
cpplogger::test::Result testLog(Context&&);

template <typename Context>
cpplogger::test::WResult testWideLog(Context&&);

}  // namespace cpplogger::test

int main() {
  if (auto result = cpplogger::test::testFormatTo(); !result) {
    return cpplogger::test::printError(result.error());
  }
  if (auto result = cpplogger::test::testWideFormatTo(); !result) {
    return cpplogger::test::printError(result.error());
  }
  cpplogger::BasicLogContext context{std::source_location::current()};
  if (auto result = cpplogger::test::testFormat(context); !result) {
    return cpplogger::test::printError(result.error());
  }
  if (auto result = cpplogger::test::testWideFormat(context); !result) {
    return cpplogger::test::printError(result.error());
  }
  if (auto result = cpplogger::test::testLog(context); !result) {
    return cpplogger::test::printError(result.error());
  }
  if (auto result = cpplogger::test::testWideLog(context); !result) {
    return cpplogger::test::printError(result.error());
  }
  return 0;
}

cpplogger::test::Result
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
    return cpplogger::test::mismatch(expect, capture.view());
  return {};
}

cpplogger::test::WResult
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
    return cpplogger::test::mismatch(expect, capture.view());
  return {};
}

template <typename Context>
cpplogger::test::Result cpplogger::test::testFormat(Context&& context) {
  cpplogger::BasicLogger<char> logger{};
  std::string actual = logger.format(context, "test log\n");
  std::stringstream expectStream{};
  logger.formatTo(
      std::ostreambuf_iterator{expectStream},
      context,
      "test log\n");
  std::string_view expect = expectStream.view();
  if (expect != actual)
    return cpplogger::test::mismatch(expect, actual);
  return {};
}

template <typename Context>
cpplogger::test::WResult cpplogger::test::testWideFormat(Context&& context) {
  cpplogger::BasicLogger<wchar_t> logger{};
  std::wstring actual = logger.format(context, L"test log\n");
  std::wstringstream expectStream{};
  logger.formatTo(
      std::ostreambuf_iterator{expectStream},
      context,
      L"test log\n");
  std::wstring_view expect = expectStream.view();
  if (expect != actual)
    return cpplogger::test::mismatch(expect, actual);
  return {};
}

template <typename Context>
cpplogger::test::Result cpplogger::test::testLog(Context&& context) {
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
    return cpplogger::test::mismatch(expect, actual);
  return {};
}

template <typename Context>
cpplogger::test::WResult cpplogger::test::testWideLog(Context&& context) {
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
    return cpplogger::test::mismatch(expect, actual);
  return {};
}
