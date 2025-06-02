#include <CppLogger/logger.hpp>

#include "Fixtures/DefaultImpl.hpp"

#include <cassert>
#include <fstream>
#include <print>

namespace test {

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
  logger::logWarn("str fmt [{}]", 10);
}

void test::testFileLog() {
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
