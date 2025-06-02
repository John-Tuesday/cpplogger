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

int main() {
  std::println("verifyLoggerCls: {}", logger::test::verifyLoggerCls());
  return 0;
}
