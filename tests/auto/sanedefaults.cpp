#include "CppLogger/message.hpp"
#include <CppLogger/concepts.hpp>
#include <CppLogger/logger.hpp>

namespace test {

template <logger::MessageType MType>
concept VerifyDefaultsFor =
    logger::concepts::FiltersLog<logger::DefaultLogFilter, MType> &&
    logger::concepts::PrintsToLog<logger::DefaultLogPrinter, MType> &&
    logger::concepts::IndirectlyProvidesLogTargets<
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
  return 0;
}
