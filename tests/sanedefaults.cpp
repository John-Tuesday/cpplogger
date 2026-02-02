#include <cpplogger/concepts.hpp>
#include <cpplogger/logger.hpp>
#include <cpplogger/message.hpp>

namespace logger::test {

bool verifyLoggerCls() {
  logger::LoggerDefaults<logger::DefaultImplTag> logger{};
  using Context = logger::LogContext<char>;
  logger.log<Context>("test output and the number ten {}", 10);
  return true;
}

}  // namespace logger::test

namespace test::ctx {

template <typename T, typename... CharTs>
concept LogHelper = (sizeof...(CharTs) >= 1) &&
                    ((logger::concepts::ProvidesLogOutputTargets<T, CharTs> &&
                      logger::concepts::FiltersLog<T, CharTs> &&
                      logger::concepts::PrintsToLog<T, CharTs>) &&
                     ...);

}  // namespace test::ctx

int main() {
  std::println("\nBegin: {}\n", "sanedefaults");
  logger::test::verifyLoggerCls();
  std::println("\nEnd: {}\n", "sanedefaults");
  return 0;
}
