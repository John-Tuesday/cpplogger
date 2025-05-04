#include "CppLogger/logger.hpp"

#include <fstream>
#include <iostream>
#include <print>
#include <source_location>
#include <tuple>

std::ostream &provideClog() { return std::clog; }

template <> struct logger::LogTargets<logger::DefaultProviderTag> {
  mutable std::ofstream m_file{"log.txt", std::ios::app};

  template <logger::MessageType M>
  auto logproviders() const noexcept -> decltype(auto) {
    return std::tuple(&provideClog,
                      [this]() -> std::ostream & { return m_file; });
  }
};

static_assert(logger::concepts::IndirectlyProvidesLogTargets<
              logger::LogTargets<logger::DefaultProviderTag>,
              logger::MessageType::Warning>);

template <>
template <logger::MessageType M, std::output_iterator<char> OutputIt>
OutputIt logger::LogFormatter<logger::DefaultFormatterTag>::format(
    const std::source_location &location, OutputIt out,
    std::string_view msg) const noexcept {
  return std::format_to(std::move(out), "[{}] {} `{}` {}:{} {}", "VOID",
                        location.file_name(), location.function_name(),
                        location.line(), location.column(), msg);
}

template <>
template <logger::MessageType M>
bool logger::LogFilter<logger::DefaultLogFilterTag>::filter(
    const std::source_location &) const noexcept {
  return true;
}

int main() {
  std::println("Start main()\n");
  {
    logger::logwarn("str fmt [{}]", 10);
  }
  {
    auto tup = std::make_tuple(1, 2, 4, 5);
    std::apply([](const auto &...vs) { (std::println("n: {}", vs), ...); },
               tup);
  }
  std::println("\nend main()\n");
  return 0;
}
