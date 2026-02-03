#include "tests/Fixtures/DefaultImpl.hpp"

#include <cpplogger/logger.hpp>

#include <cassert>
#include <string_view>

namespace logger::test {

bool testFileLog();

}

int main() {
  if (!logger::test::testFileLog())
    return 1;
  return 0;
}

/**
 * Log to a logger which writes to a file, then verify the contents of the file.
 *
 * TODO: Find a better to communicate reasons for failure
 *
 * TODO: Ensure log file is empty before writing to log.
 * TODO: Ensure log file contains ONLY what is expected and nothing else.
 */
bool logger::test::testFileLog() {
  using Context = logger::InfoContext<char>;
  constexpr std::string_view expect = "info: 5 == 5";
  logger::test::LogTargetsBasicFileLog logger{};
  logger.log<Context>("info: 5 == {}", 5);
  std::ifstream logIn{logger::test::LogTargetsBasicFileLog::logPath()};
  for (std::string line; std::getline(logIn, line, '\n');) {
    if (!line.contains(expect)) {
      std::println("!!!FAIL!!!");
      std::println("  expect: '{}'", expect);
      std::println("  actual: '{}'", line);
      return false;
    } else {
      std::println("PASS");
      return true;
    }
  }
  // TODO: Communicate that this failed because no line was read
  return false;
}
