#include "CppLogger/message.hpp"
#include <CppLogger/concepts.hpp>
#include <CppLogger/logger.hpp>

#include "tests/Fixtures/DefaultImpl.hpp"

template <>
struct logger::LoggerDefaults<logger::DefaultImplTag>
    : public logger::LoggerDefaults<void> {

  template <logger::concepts::LogContextLike Context>
    requires std::same_as<logger::LogContextCharType<Context>, char>
  auto targets(const Context &) noexcept
      -> logger::concepts::TupleLikeOfLogTargets<
          LogContextCharType<Context>> decltype(auto) {
    return std::tuple(std::ref(std::cerr));
  }

  template <logger::concepts::LogContextLike Context>
    requires std::same_as<logger::LogContextCharType<Context>, wchar_t>
  auto targets(const Context &) noexcept
      -> logger::concepts::TupleLikeOfLogTargets<
          LogContextCharType<Context>> decltype(auto) {
    return std::tuple(std::ref(std::wcerr));
  }
};

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

void runTest() {
  std::println("running ctx...");
  {
    static_assert(LogHelper<logger::DefaultLogger_, char>);
    static_assert(LogHelper<logger::TemplLogger, char>);
    static_assert(
        LogHelper<logger::LoggerDefaults<logger::DefaultImplTag>, char>);
  }
  using Cxt = logger::MTypeContext<logger::MessageType::Info, char>;
  {
    logger::DefaultLogger_ deflog{};
    deflog.log<Cxt>("Def log: {}", 5);
  }
  {
    logger::TemplLogger temlog{};
    temlog.log<logger::MTypeContext<logger::MessageType::Debug, char>>(
        "Templ log(Debug): {}", 5);
    temlog.log<Cxt>("Templ log(default): {}", 5);
  }
  {
    logger::LoggerDefaults<logger::DefaultImplTag> injLog{};
    injLog.log<Cxt>("Inj log: {}", 5);
    injLog.log<logger::MTypeContext<logger::MessageType::Debug, wchar_t>>(
        L"Inj log: {}", 5);
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
