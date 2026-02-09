#include <cpplogger/concepts.hpp>
#include <cpplogger/logger.hpp>
#include <cpplogger/message.hpp>

#include <print>

namespace logger::test {

bool verifyLoggerCls() {
  cpplogger::DefaultLogger<cpplogger::DefaultTag>::Logger<char> logger{};
  using Context = cpplogger::LogContext;
  Context context = {std::source_location::current()};
  logger.log(context, "test output and the number ten {}", 10);
  return true;
}

}  // namespace logger::test

int main() {
  std::println("\nBegin: {}\n", "sanedefaults");
  logger::test::verifyLoggerCls();
  std::println("\nEnd: {}\n", "sanedefaults");
  return 0;
}
