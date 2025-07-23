#include "CppLogger/message.hpp"
#include <CppLogger/concepts.hpp>
#include <CppLogger/logger.hpp>

#include "tests/Fixtures/DefaultImpl.hpp"

namespace logger::test {

struct LoggerClsTests {
  bool test() {
    logger::LoggerDefaults<logger::DefaultImplTag> logger{};
    using Context = logger::LogContext<char>;
    logger.log<Context>("Foopy {}", 10);
    logger::test::DoubleCerrLogger cust{};
    cust.log<Context>("Foopy {}", 10);
    logger::test::ChainLogger<logger::test::DoubleCerrLogger> chain{};
    chain.log<Context>("Foopy {}", 10);
    return true;
  }
};

bool verifyLoggerCls() {
  LoggerClsTests tester{};
  return tester.test();
}

} // namespace logger::test

namespace test::ctx {

template <typename T, typename... CharTs>
concept LogHelper = (sizeof...(CharTs) >= 1) &&
                    ((logger::concepts::ProvidesLogOutputTargets<T, CharTs> &&
                      logger::concepts::FiltersLog<T, CharTs> &&
                      logger::concepts::PrintsToLog<T, CharTs>) &&
                     ...);

} // namespace test::ctx

int main() {
  std::println("\nBegin: {}\n", "sanedefaults");
  logger::test::verifyLoggerCls();
  std::println("\nEnd: {}\n", "sanedefaults");
  return 0;
}
