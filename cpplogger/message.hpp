#pragma once

#ifndef CPPLOGGER_MESSAGE_HPP
#define CPPLOGGER_MESSAGE_HPP

#include <concepts>
#include <format>
#include <source_location>

namespace cpplogger {

struct LogContext;
struct FatalContext;
struct ErrorContext;
struct WarningContext;
struct InfoContext;
struct DebugContext;
struct VerboseContext;

}  // namespace cpplogger

struct cpplogger::LogContext : public std::source_location {};

struct cpplogger::FatalContext : public cpplogger::LogContext {};

struct cpplogger::ErrorContext : public cpplogger::LogContext {};

struct cpplogger::WarningContext : public cpplogger::LogContext {};

struct cpplogger::InfoContext : public cpplogger::LogContext {};

struct cpplogger::DebugContext : public cpplogger::LogContext {};

struct cpplogger::VerboseContext : public cpplogger::LogContext {};

namespace cpplogger {

template <
    std::constructible_from<std::source_location> Context,
    typename CharT,
    typename... Args>
class LogContextFormatString;

}  // namespace cpplogger

/**
 * @brief Intended to be used a drop-in replacement for
 * `std::basic_format_string<>` but with extra context information for logging
 * purposes.
 *
 * @todo Consider letting the context variable be mutable.
 * @todo The context getter should reflect the mutability of the context.
 * @todo Add constraints like `std::is_trivially_constructible` to the `Context`
 * type depending on if context is meant to be copied or referenced.
 */
template <
    std::constructible_from<std::source_location> Ctx,
    typename CharT,
    typename... Args>
class cpplogger::LogContextFormatString
    : public std::basic_format_string<CharT, std::type_identity_t<Args>...> {
 public:
  using Context = Ctx;

  /**
   * @brief Intended to be used a drop-in replacement for
   * `std::basic_format_string<>`.
   *
   * Forwards `fmt` to `std::basic_format_string<>`.
   *
   * @nore The default value for `location` will describe the callsite.
   *
   * @param[in] fmt forwarded as the first argument of the constructor for
   * `std::basic_format_string<>`
   * @param[in] location used to initialize the associated context variable
   */
  template <typename T>
  consteval LogContextFormatString(
      T&& fmt,
      std::source_location location = std::source_location::current())
      : std::basic_format_string<
            CharT,
            std::type_identity_t<Args>...>{std::forward<T>(fmt)},
        m_context{location} {}

  /**
   * @return the associated context.
   */
  template <typename Self>
  constexpr auto context(this Self&& self) -> decltype(auto) {
    return self.m_context;
    // return std::forward_like<Self>(self.m_context);
  }

 private:
  Context m_context{};
};

#endif
