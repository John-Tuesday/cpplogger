#ifndef CPPLOGGER_LOG_HPP
#define CPPLOGGER_LOG_HPP

#include <format>

#include "Defaults.hpp"
#include "LogContextFormatString.hpp"
#include "context.hpp"

namespace cpplogger {

/**
 * @brief Format and write log message using default constructed `LoggerType`.
 */
template <
    typename LoggerType =
        cpplogger::Defaults<cpplogger::DefaultTag>::Logger<char>,
    typename Context,
    typename... Args>
void log(
    Context&& context,
    std::basic_format_string<char, std::type_identity_t<Args>...> fmt,
    Args&&... args) {
  LoggerType{}.log(
      std::forward<Context>(context),
      fmt,
      std::forward<Args>(args)...);
}

/**
 * @copybrief `cpplogger::log()`
 * @brief Uses the default fatal context.
 */
template <
    typename LoggerType =
        cpplogger::Defaults<cpplogger::DefaultTag>::Logger<char>,
    typename... Args>
void logFatal(
    cpplogger::LogContextFormatString<
        cpplogger::FatalContext,
        char,
        std::type_identity_t<Args>...> fmt,
    Args&&... args) {
  LoggerType{}.log(fmt.context(), fmt, std::forward<Args>(args)...);
}

/**
 * @copybrief `cpplogger::log()`
 * @brief Uses the default error context.
 */
template <
    typename LoggerType =
        cpplogger::Defaults<cpplogger::DefaultTag>::Logger<char>,
    typename... Args>
void logError(
    cpplogger::LogContextFormatString<
        cpplogger::ErrorContext,
        char,
        std::type_identity_t<Args>...> fmt,
    Args&&... args) {
  LoggerType{}.log(fmt.context(), fmt, std::forward<Args>(args)...);
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
  using LoggerType = cpplogger::Defaults<cpplogger::DefaultTag>::Logger<char>;
  LoggerType{}.log(fmt.context(), fmt, std::forward<Args>(args)...);
}

/**
 * @copydoc `cpplogger::logWarning()`
 */
template <
    typename LoggerType =
        cpplogger::Defaults<cpplogger::DefaultTag>::Logger<char>,
    typename... Args>
void logWarn(
    cpplogger::LogContextFormatString<
        cpplogger::WarningContext,
        char,
        std::type_identity_t<Args>...> fmt,
    Args&&... args) {
  LoggerType{}.log(fmt.context(), fmt, std::forward<Args>(args)...);
}

/**
 * @copybrief `cpplogger::log()`
 * @brief Uses the default info context.
 */
template <
    typename LoggerType =
        cpplogger::Defaults<cpplogger::DefaultTag>::Logger<char>,
    typename... Args>
void logInfo(
    cpplogger::LogContextFormatString<
        cpplogger::InfoContext,
        char,
        std::type_identity_t<Args>...> fmt,
    Args&&... args) {
  LoggerType{}.log(fmt.context(), fmt, std::forward<Args>(args)...);
}

/**
 * @copybrief `cpplogger::log()`
 * @brief Uses the default debug context.
 */
template <
    typename LoggerType =
        cpplogger::Defaults<cpplogger::DefaultTag>::Logger<char>,
    typename... Args>
void logDebug(
    cpplogger::LogContextFormatString<
        cpplogger::DebugContext,
        char,
        std::type_identity_t<Args>...> fmt,
    Args&&... args) {
  LoggerType{}.log(fmt.context(), fmt, std::forward<Args>(args)...);
}

/**
 * @copybrief `cpplogger::log()`
 * @brief Uses the default verbose context.
 */
template <
    typename LoggerType =
        cpplogger::Defaults<cpplogger::DefaultTag>::Logger<char>,
    typename... Args>
void logVerbose(
    cpplogger::LogContextFormatString<
        cpplogger::VerboseContext,
        char,
        std::type_identity_t<Args>...> fmt,
    Args&&... args) {
  LoggerType{}.log(fmt.context(), fmt, std::forward<Args>(args)...);
}

}  // namespace cpplogger

#endif
