#include "CppLogger/message.hpp"
#include <CppLogger/concepts.hpp>
#include <CppLogger/logger.hpp>

#include "tests/Fixtures/DefaultImpl.hpp"

template <>
template <logger::concepts::LogContextFrom Context>
auto logger::LoggerDefaults<logger::DefaultImplTag>::targets(
    const Context &) noexcept -> logger::concepts::TupleLikeOfLogTargets
    decltype(auto) {
  return std::tuple(std::ref(std::cerr));
}

namespace logger::test {

struct LoggerClsTests {
  bool test() {
    logger::LoggerDefaults<logger::DefaultImplTag> logger{};
    logger.log("Foopy {}", 10);
    logger::test::DoubleCerrLogger cust{};
    cust.log("Foopy {}", 10);
    logger::test::ChainLogger<logger::test::DoubleCerrLogger> chain{};
    chain.log("Foopy {}", 10);
    return true;
  }
};

bool verifyLoggerCls() {
  LoggerClsTests tester{};
  return tester.test();
}

} // namespace logger::test

namespace test::ctx {

template <typename T>
concept LogHelper =
    logger::concepts::ProvidesLogOutputTargets<T> &&
    logger::concepts::FiltersLog<T> && logger::concepts::PrintsToLog<T>;

void runTest() {
  std::println("running ctx...");
  {
    static_assert(LogHelper<logger::DefaultLogger_>);
    static_assert(LogHelper<logger::TemplLogger>);
    static_assert(LogHelper<logger::LoggerDefaults<logger::DefaultImplTag>>);
  }
  {
    logger::DefaultLogger_ deflog{};
    deflog.log("Def log: {}", 5);
  }
  {
    logger::TemplLogger temlog{};
    temlog.log<logger::MTypeContext<logger::MessageType::Debug>>(
        "Templ log(Debug): {}", 5);
    temlog.log("Templ log(default): {}", 5);
  }
  {
    logger::LoggerDefaults<logger::DefaultImplTag> injLog{};
    injLog.log("Inj log: {}", 5);
  }
  std::println("finished ctx!");
}

} // namespace test::ctx

int main() {
  std::println("\nBegin: {}\n", "sanedefaults");
  test::ctx::runTest();
  std::println("\nEnd: {}\n", "sanedefaults");
  return 0;
}
