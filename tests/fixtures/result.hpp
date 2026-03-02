#ifndef TESTS_FIXTURES_RESULT_HPP
#define TESTS_FIXTURES_RESULT_HPP

#include <expected>
#include <format>
#include <iostream>
#include <source_location>
#include <sstream>
#include <string>

namespace cpplogger::test {

template <typename CharT>
using BasicResult = std::expected<void, std::basic_string<CharT>>;

using Result = std::expected<void, std::string>;
using ResultFail = std::unexpected<std::string>;
using WResult = std::expected<void, std::wstring>;
using WResultFail = std::unexpected<std::wstring>;

inline std::string mismatchErrorMessage(
    std::string_view expect,
    std::string_view actual,
    std::source_location location = std::source_location::current());

inline std::wstring mismatchErrorMessage(
    std::wstring_view expect,
    std::wstring_view actual,
    std::source_location location = std::source_location::current());

inline Result mismatch(
    std::string_view expect,
    std::string_view actual,
    std::source_location location = std::source_location::current());

inline WResult mismatch(
    std::wstring_view expect,
    std::wstring_view actual,
    std::source_location location = std::source_location::current());

inline int printError(std::string_view message);
inline int printError(std::wstring_view message);

}  // namespace cpplogger::test

inline std::string cpplogger::test::mismatchErrorMessage(
    std::string_view expect,
    std::string_view actual,
    std::source_location location) {
  return std::format(
      "function: {}\n"
      "expect: '{}'\n"
      "actual: '{}'",
      location.function_name(),
      expect,
      actual);
}

inline std::wstring cpplogger::test::mismatchErrorMessage(
    std::wstring_view expect,
    std::wstring_view actual,
    std::source_location location) {
  std::wstringstream os{};
  auto it = std::format_to(
      std::ostreambuf_iterator{os},
      "function: {}\n",
      location.function_name());
  it = std::format_to(
      it,
      L"expect: '{}'\n"
      L"actual: '{}'",
      expect,
      actual);
  return std::move(os).str();
}

inline cpplogger::test::Result cpplogger::test::mismatch(
    std::string_view expect,
    std::string_view actual,
    std::source_location location) {
  return cpplogger::test::ResultFail(
      cpplogger::test::mismatchErrorMessage(expect, actual, location));
}

inline cpplogger::test::WResult cpplogger::test::mismatch(
    std::wstring_view expect,
    std::wstring_view actual,
    std::source_location location) {
  return cpplogger::test::WResultFail(
      cpplogger::test::mismatchErrorMessage(expect, actual, location));
}

inline int cpplogger::test::printError(std::string_view message) {
  std::println(std::cerr, "{}", message);
  return 1;
}

inline int cpplogger::test::printError(std::wstring_view message) {
  std::wcerr << message << "\n";
  return 1;
}

#endif
