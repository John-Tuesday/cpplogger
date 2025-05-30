#pragma once

#include "concepts.hpp"
#include "functors.hpp"
#include "message.hpp"

#include <format>
#include <syncstream>

/** Base namespace. */
namespace logger {

/**
 * Base logging implementation.
 */
template <
    MessageType MType,
    concepts::IndirectlyProvidesLogTargets<MType> Providers =
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
              &printer]<concepts::ProvidesLogTarget... Ps>(Ps &&...ps) {
    (
        [&](auto &&s) {
          std::osyncstream sync{s};
          printer.template print<MType>(sync, location, message);
        }(ps()),
        ...);
  };
  using ProvidersType =
      decltype(std::declval<Providers>().template providers<MType>());
  if constexpr (concepts::TupleLikeLogTargetProviders<ProvidersType>) {
    std::apply(fns, Providers{}.template providers<MType>());
  } else if constexpr (concepts::LogTargetProviderRange<ProvidersType>) {
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
