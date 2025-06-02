#include "TestFileLog.hpp"

#include "tests/Fixtures/DefaultImpl.hpp"

#include <cassert>
#include <string_view>

void logger::test::TestFileLog::test() {
  constexpr auto mtype = logger::MessageType::Info;
  constexpr std::string_view expect = "info: 5 == 5";
  logger::log<logger::MTypeContext<mtype>,
              logger::test::LogTargetsBasicFileLog>("info: 5 == {}", 5);
  std::ifstream logIn{logger::test::LogTargetsBasicFileLog::logPath()};
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

void logger::test::testFileLog() { TestFileLog{}.test(); }

int main() {
  logger::test::testFileLog();
  return 0;
}
