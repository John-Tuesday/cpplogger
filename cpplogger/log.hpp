#ifndef CPPLOGGER_LOG_HPP
#define CPPLOGGER_LOG_HPP

#include "Defaults.hpp"
#include "LogContextFormatString.hpp"
#include "context.hpp"

namespace cpplogger {

/**
 * @brief Format and write log message using default constructed `LoggerType`.
 */
template <
    typename Context,
    typename Tag = cpplogger::DefaultTag,
    typename... Args>
void log(
    cpplogger::
        LogContextFormatString<Context, char, std::type_identity_t<Args>...>
            fmt,
    Args&&... args) {
  // This forces Logger to be a dependent name, thus delaying resolution of
  // Defaults and most importantly delaying the instantiation of a
  // specialization.
  using Logger = typename cpplogger::Defaults<Tag>::template Logger<char>;
  Logger().log(fmt.context(), fmt, std::forward<Args>(args)...);
}

/**
 * @copybrief `cpplogger::log()`
 * @brief Uses the default fatal context.
 */
template <typename... Args>
void logFatal(
    cpplogger::LogContextFormatString<
        cpplogger::FatalContext,
        char,
        std::type_identity_t<Args>...> fmt,
    Args&&... args) {
  cpplogger::log(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * @copybrief `cpplogger::log()`
 * @brief Uses the default error context.
 */
template <typename... Args>
void logError(
    cpplogger::LogContextFormatString<
        cpplogger::ErrorContext,
        char,
        std::type_identity_t<Args>...> fmt,
    Args&&... args) {
  cpplogger::log(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * @copybrief `cpplogger::log()`
 * @brief Uses the default warning context.
 */
template <typename... Args>
void logWarning(
    cpplogger::LogContextFormatString<
        cpplogger::WarningContext,
        char,
        std::type_identity_t<Args>...> fmt,
    Args&&... args) {
  cpplogger::log(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * @copydoc `cpplogger::logWarning()`
 */
template <typename... Args>
void logWarn(
    cpplogger::LogContextFormatString<
        cpplogger::WarningContext,
        char,
        std::type_identity_t<Args>...> fmt,
    Args&&... args) {
  cpplogger::log(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * @copybrief `cpplogger::log()`
 * @brief Uses the default info context.
 */
template <typename... Args>
void logInfo(
    cpplogger::LogContextFormatString<
        cpplogger::InfoContext,
        char,
        std::type_identity_t<Args>...> fmt,
    Args&&... args) {
  cpplogger::log(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * @copybrief `cpplogger::log()`
 * @brief Uses the default debug context.
 */
template <typename... Args>
void logDebug(
    cpplogger::LogContextFormatString<
        cpplogger::DebugContext,
        char,
        std::type_identity_t<Args>...> fmt,
    Args&&... args) {
  cpplogger::log(std::move(fmt), std::forward<Args>(args)...);
}

/**
 * @copybrief `cpplogger::log()`
 * @brief Uses the default verbose context.
 */
template <typename... Args>
void logVerbose(
    cpplogger::LogContextFormatString<
        cpplogger::VerboseContext,
        char,
        std::type_identity_t<Args>...> fmt,
    Args&&... args) {
  cpplogger::log(std::move(fmt), std::forward<Args>(args)...);
}

}  // namespace cpplogger

#endif
