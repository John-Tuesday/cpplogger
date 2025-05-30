#include "CppLogger/message.hpp"
#include <CppLogger/concepts.hpp>
#include <CppLogger/logger.hpp>

#include "tests/Fixtures/DefaultImpl.hpp"

namespace logger::test {

struct LoggerClsTests {
  template <MessageType MType = MessageType::Info> bool test() {
    logger::DefaultLogger logger{};
    logger.log<MType>("Foopy {}", 10);
    logger::test::DoubleCerrLogger cust{};
    cust.log<MType>("Foopy {}", 10);
    logger::test::ChainLogger<logger::test::DoubleCerrLogger> chain{};
    chain.log<MType>("Foopy {}", 10);
    return true;
  }
};

bool verifyLoggerCls() {
  LoggerClsTests tester{};
  return tester.test();
}

} // namespace logger::test

namespace test {

template <logger::MessageType MType>
concept VerifyDefaultsFor =
    logger::concepts::FiltersLog<logger::DefaultLogFilter, MType> &&
    logger::concepts::PrintsToLog<logger::DefaultLogPrinter, MType> &&
    logger::concepts::ProvidesIndirectLogTargets<
        logger::DefaultLogTargetProviders, MType>;

template <logger::MessageType... MTypes> constexpr bool testTypes() noexcept {
  return (VerifyDefaultsFor<MTypes> && ...);
}

constexpr bool verifyDefaults() {
  using enum logger::MessageType;
  return test::testTypes<Fatal, Error, Warning, Info, Debug, Verbose>();
}

} // namespace test

int main() {
  constexpr bool result = test::verifyDefaults();

  std::println("verifyLoggerCls: {}", logger::test::verifyLoggerCls());
  return 0;
}
