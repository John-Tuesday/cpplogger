#pragma once

#include "concepts.hpp"
#include "functors.hpp"
#include "message.hpp"

#include <format>
#include <syncstream>

/** Base namespace. */
namespace logger {

struct Logger;
struct DefaultLogger;

struct Logger {
  template <MessageType MType, typename Self>
  void write(this Self &&self, std::string_view message,
             const std::source_location &location) {
    if constexpr (logger::concepts::FiltersLog<Self, MType>) {
      if (!self.template filter<MType>(location))
        return;
    }
    if constexpr (logger::concepts::ProvidesLogOutputTargets<Self, MType> &&
                  logger::concepts::PrintsToLog<Self, MType>) {
      std::apply(
          [&self, &location,
           &message]<logger::concepts::LogTarget... Ts>(Ts &&...ts) {
            (self.template print<MType>(std::osyncstream{ts}, location,
                                        message),
             ...);
          },
          self.template targets<MType>(location));
    }
  }

  template <MessageType MType, typename Self, typename... Args>
  void log(this Self &&self, LogFormatString<std::type_identity_t<Args>...> fmt,
           Args &&...args) {
    self.template write<MType>(std::format(fmt, std::forward<Args>(args)...),
                               fmt.location());
  }
};

struct DefaultLogger : public logger::Logger {
  template <MessageType MType>
  auto targets(this auto &&self, const std::source_location &location) noexcept
      -> logger::concepts::TupleLikeOfLogTargets decltype(auto) {
    return (typename logger::MessageTypeTraits<MType>::Targets){}
        .template targets<MType>(location);
  }

  template <MessageType MType>
  bool filter(const std::source_location &location) const noexcept {
    return (typename logger::MessageTypeTraits<MType>::Filter){}
        .template filter<MType>(location);
  }

  template <MessageType MType, logger::concepts::LogTarget Stream>
  void print(this auto &&self, Stream &&stream,
             const std::source_location &location,
             std::string_view message) noexcept {
    return (typename logger::MessageTypeTraits<MType>::Printer){}
        .template print<MType>(std::forward<Stream>(stream), location, message);
  }
};

/**
 * Base logging implementation.
 */
template <
    MessageType MType,
    concepts::ProvidesIndirectLogTargets<MType> Providers =
        MessageTypeTraits<MType>::TargetProvider,
    concepts::PrintsToLog<MType> Printer = MessageTypeTraits<MType>::Printer,
    concepts::FiltersLog<MType> Filter = MessageTypeTraits<MType>::Filter,
    typename... Args>
void log(LogFormatString<std::type_identity_t<Args>...> fmt,
         Args &&...args) noexcept {
  if (!Filter{}.template filter<MType>(fmt.location()))
    return;
  std::string message =
      std::format(std::move(fmt), std::forward<Args>(args)...);
  Printer printer{};
  auto fns = [&message, location = fmt.location(),
              &printer]<concepts::IndirectlyProvidesLogTarget... Ps>(
                 Ps &&...ps) {
    (
        [&](auto &&s) {
          printer.template print<MType>(std::osyncstream{s}, location, message);
        }(ps()),
        ...);
  };
  using ProvidersType =
      decltype(std::declval<Providers>().template providers<MType>());
  if constexpr (concepts::TupleLikeOfIndirectLogTargets<ProvidersType>) {
    std::apply(fns, Providers{}.template providers<MType>());
  } else if constexpr (concepts::RangeOfIndirectLogTargets<ProvidersType>) {
    for (auto p : Providers{}.template providers<MType>()) {
      fns(p());
    }
  }
}

/**
 * Log using defaults for `MessageType::Fatal`.
 *
 * @param[in] fmt format string input
 * @param[in] args variables to formated
 */
template <typename... Args>
void logFatal(LogFormatString<std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MessageType::Fatal>(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Error`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logError(LogFormatString<std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MessageType::Error>(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Warning`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logWarn(LogFormatString<std::type_identity_t<Args>...> fmt,
             Args &&...args) noexcept {
  log<MessageType::Warning>(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Info`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logInfo(LogFormatString<std::type_identity_t<Args>...> fmt,
             Args &&...args) noexcept {
  log<MessageType::Info>(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Debug`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logDebug(LogFormatString<std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept {
  log<MessageType::Debug>(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * Log using defaults for `MessageType::Verbose`.
 *
 * @copydetails logFatal()
 */
template <typename... Args>
void logVerbose(LogFormatString<std::type_identity_t<Args>...> fmt,
                Args &&...args) noexcept {
  log<MessageType::Verbose>(std::move(fmt), std::forward<Args>(args)...);
}

} // namespace logger
