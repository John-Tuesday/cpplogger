#include "defaults_unit_test.hpp"

#include <print>

int main() {
  static_assert(
      std::same_as<
          cpplogger::Defaults<cpplogger::DefaultTag>::Logger<char>,
          cpplogger::test::AltLogger>,
      "Injected type is not as specified.");
  cpplogger::Defaults<cpplogger::DefaultTag>::Logger<char> logger{};
  cpplogger::BasicLogContext context{};
  std::string expect = logger.format(context, "log message\n");
  std::stringstream capture{};
  auto old = std::cerr.rdbuf(capture.rdbuf());
  cpplogger::log(context, "log message");
  std::cerr.rdbuf(old);
  std::string_view actual = capture.view();
  if (actual != expect) {
    std::println(
        "Failed!\n"
        "actual: '{}'\n"
        "expect: '{}'",
        actual,
        expect);
    return 1;
  }
  return 0;
}
