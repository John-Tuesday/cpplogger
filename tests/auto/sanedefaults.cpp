#include <CppLogger/concepts.hpp>
#include <CppLogger/logger.hpp>

int main() {
  constexpr auto MType = logger::MessageType::Info;
  static_assert(logger::concepts::IndirectlyProvidesLogTargets<
                logger::LogTargets<logger::DefaultProviderTag>, MType>);
}
