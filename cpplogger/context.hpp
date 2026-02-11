#ifndef CPPLOGGER_CONTEXT_HPP
#define CPPLOGGER_CONTEXT_HPP

#include "concepts.hpp"

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

template <>
struct std::formatter<cpplogger::LogContext, char>;

template <typename T>
  requires std::derived_from<T, cpplogger::LogContext> &&
           cpplogger::CategorizedLogContext<T, char>
struct std::formatter<T, char>;

/**
 * @brief Root class which provides context to logging functions.
 */
struct cpplogger::LogContext : public std::source_location {
  /**
   * @brief Default initialization, follows `std::source_location` default
   * initialization.
   */
  constexpr LogContext() noexcept : std::source_location{} {}

  /**
   * @note Although this function has been marked `constexpr`, the standard does
   * not specifiy if the `std::source_location` is `constexpr` or not.
   * @note This function not `noexcept` because neither is the copy constructor
   * of `std::source_location`.
   */
  constexpr LogContext(const std::source_location& location)
      : std::source_location{location} {}

  /**
   * @note Although this function has been marked `constexpr`, the standard does
   * not specifiy if the `std::source_location` is `constexpr` or not.
   */
  constexpr LogContext(std::source_location&& location) noexcept
      : std::source_location{std::move(location)} {}
};

struct cpplogger::FatalContext : public cpplogger::LogContext {
  static constexpr std::string_view category = "fatal";
};

struct cpplogger::ErrorContext : public cpplogger::LogContext {
  static constexpr std::string_view category = "error";
};

struct cpplogger::WarningContext : public cpplogger::LogContext {
  static constexpr std::string_view category = "warn";
};

struct cpplogger::InfoContext : public cpplogger::LogContext {
  static constexpr std::string_view category = "info";
};

struct cpplogger::DebugContext : public cpplogger::LogContext {
  static constexpr std::string_view category = "debug";
};

struct cpplogger::VerboseContext : public cpplogger::LogContext {
  static constexpr std::string_view category = "verbose";
};

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

/**
 * @brief Format basic context information.
 */
template <>
struct std::formatter<cpplogger::LogContext, char> {

  template <typename ParseContext>
  constexpr ParseContext::iterator parse(ParseContext& context) {
    auto it = context.begin();
    if (it != context.end() && *it != '}') {
      throw std::format_error("Unrecognized format args");
    }
    return it;
  }

  template <typename FormatContext>
  FormatContext::iterator
  format(cpplogger::LogContext logContext, FormatContext& fmtContext) const {
    auto it = fmtContext.out();
    auto end = std::format_to(
        it,
        "{}: {}:{} `{}`",
        logContext.file_name(),
        logContext.line(),
        logContext.column(),
        logContext.function_name());
    return end;
  }
};

/**
 * @brief Format like `cpplogger::LogContext` but include category information.
 */
template <typename T>
  requires std::derived_from<T, cpplogger::LogContext> &&
           cpplogger::CategorizedLogContext<T, char>
struct std::formatter<T> : public std::formatter<cpplogger::LogContext, char> {
  using BaseFormatter = std::formatter<cpplogger::LogContext, char>;

  using BaseFormatter::parse;

  template <typename FormatContext>
  FormatContext::iterator
  format(T logContext, FormatContext& fmtContext) const {
    auto it = fmtContext.out();
    const std::source_location& location = logContext;
    it = std::format_to(
        it,
        "[{}] {}",
        decltype(logContext)::category,
        static_cast<cpplogger::LogContext&>(logContext));
    return it;
  }
};

#endif
