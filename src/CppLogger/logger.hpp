#pragma once

#include "concepts.hpp"

#include <format>
#include <iostream>
#include <iterator>
#include <ranges>
#include <source_location>
#include <string_view>
#include <syncstream>

namespace logger {

enum class MessageType {
  Fatal,
  Error,
  Warning,
  Info,
  Debug,
  Verbose,
};

template <typename... Args> struct LogFormatString;

template <typename> struct LogTargets;
struct DefaultProviderTag {};
template <typename> struct LogFormatter;
struct DefaultFormatterTag {};
template <typename> struct LogFilter;
struct DefaultLogFilterTag {};

template <MessageType> struct MessageTypeTraits;

template <typename... Args>
struct LogFormatString
    : public std::format_string<std::type_identity_t<Args>...> {
  using ParentType = std::format_string<std::type_identity_t<Args>...>;

  template <typename T>
  consteval LogFormatString(
      T &&fmt, std::source_location location = std::source_location::current())
      : ParentType(std::forward<T>(fmt)), m_location(location) {}

  constexpr std::source_location location() const noexcept {
    return m_location;
  }

private:
  std::source_location m_location{};
};

template <typename T> struct LogTargets {
  template <MessageType M>
  auto logproviders() const noexcept -> decltype(auto) {
    return std::ranges::empty_view<decltype([]() -> std::ostream & {
      return std::declval<std::ostream &>();
    })>{};
  }
};

template <typename T> struct LogFormatter {
  template <MessageType M, std::output_iterator<char> OutputIt>
  OutputIt format(const std::source_location &, OutputIt out,
                  std::string_view msg) const noexcept {
    return std::format_to(std::move(out), "{}", msg);
  }
};

template <typename T> struct LogFilter {
  template <MessageType M>
  bool filter(const std::source_location &) const noexcept {
    return true;
  }
};

template <MessageType M> struct MessageTypeTraits {
  static constexpr MessageType type = M;

  using TargetProvider = LogTargets<DefaultProviderTag>;
  using Formatter = LogFormatter<DefaultFormatterTag>;
  using Filter = LogFilter<DefaultLogFilterTag>;
  static_assert(concepts::IndirectlyProvidesLogTargets<TargetProvider, M>);
  static_assert(concepts::IsLogFormatter<Formatter, M>);
  static_assert(concepts::IsLogFilter<Filter, M>);
};

template <MessageType type,
          concepts::IndirectlyProvidesLogTargets<type> Provider =
              MessageTypeTraits<type>::TargetProvider,
          concepts::IsLogFormatter<type> Formatter =
              MessageTypeTraits<type>::Formatter,
          concepts::IsLogFilter<type> Filter = MessageTypeTraits<type>::Filter,
          typename... Args>
void log(LogFormatString<std::type_identity_t<Args>...> fmt,
         Args &&...args) noexcept {
  if (!Filter{}.template filter<type>(fmt.location()))
    return;
  std::ostringstream strStream{};
  Formatter{}.template format<type>(
      fmt.location(), std::ostream_iterator<char>(strStream),
      std::format(std::move(fmt), std::forward<Args>(args)...));
  for (const auto &p : Provider{}.template logproviders<type>()) {
    std::osyncstream{p()} << strStream.str() << '\n';
  }
}

template <typename... Args>
void logfatal(LogFormatString<std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MessageType::Fatal>(std::move(fmt), std::forward<Args>(args)...);
}

template <typename... Args>
void logerror(LogFormatString<std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MessageType::Error>(std::move(fmt), std::forward<Args>(args)...);
}

template <typename... Args>
void logwarn(LogFormatString<std::type_identity_t<Args>...> fmt,
             Args &&...args) noexcept {
  log<MessageType::Warning>(std::move(fmt), std::forward<Args>(args)...);
}

template <typename... Args>
void loginfo(LogFormatString<std::type_identity_t<Args>...> fmt,
             Args &&...args) noexcept {
  log<MessageType::Info>(std::move(fmt), std::forward<Args>(args)...);
}

template <typename... Args>
void logdebug(LogFormatString<std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MessageType::Debug>(std::move(fmt), std::forward<Args>(args)...);
}

template <typename... Args>
void logverbose(LogFormatString<std::type_identity_t<Args>...> fmt,
                Args &&...args) noexcept {
  log<MessageType::Verbose>(std::move(fmt), std::forward<Args>(args)...);
}

} // namespace logger
