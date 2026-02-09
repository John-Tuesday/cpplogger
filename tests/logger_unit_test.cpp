#include <cpplogger/logger.hpp>
#include <cpplogger/message.hpp>

#include <expected>
#include <print>
#include <sstream>

namespace cpplogger::test {

std::expected<void, std::string> testFormatTo();

}

int main() {
  if (std::expected result = cpplogger::test::testFormatTo(); !result) {
    std::println(std::cerr, "Failed!\n{}", result.error());
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
