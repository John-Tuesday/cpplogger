#include "CppLogger/logger.hpp"

#include <iostream>
#include <print>
#include <source_location>

std::ostream &provideClog() { return std::clog; }

template <>
template <logger::MessageType M>
auto logger::LogTargets<logger::DefaultProviderTag>::logproviders()
    const noexcept -> decltype(auto) {
  static constexpr std::ranges::single_view view{&provideClog};
  return view;
}

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
  std::println("\nend main()\n");
  return 0;
}
