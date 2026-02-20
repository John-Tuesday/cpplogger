#ifndef CPPLOGGER_CONTEXT_HPP
#define CPPLOGGER_CONTEXT_HPP

#include <concepts>
#include <format>
#include <source_location>

namespace cpplogger {

struct BasicLogContext;
struct FatalContext;
struct ErrorContext;
struct WarningContext;
struct InfoContext;
struct DebugContext;
struct VerboseContext;

}  // namespace cpplogger

template <typename T>
  requires std::derived_from<T, cpplogger::BasicLogContext>
struct std::formatter<T, char>;

/**
 * @brief Root class which provides context to logging functions.
 */
struct cpplogger::BasicLogContext : public std::source_location {
  /**
   * @brief Default initialization, follows `std::source_location` default
   * initialization.
   */
  constexpr BasicLogContext() noexcept
      : std::source_location{}, m_empty{true} {}

  /**
   * @note Although this function has been marked `constexpr`, the standard does
   * not specifiy if the `std::source_location` is `constexpr` or not.
   * @note This function not `noexcept` because neither is the copy constructor
   * of `std::source_location`.
   */
  constexpr BasicLogContext(const std::source_location& location)
      : std::source_location{location} {}

  /**
   * @note Although this function has been marked `constexpr`, the standard does
   * not specifiy if the `std::source_location` is `constexpr` or not.
   */
  constexpr BasicLogContext(std::source_location&& location) noexcept
      : std::source_location{std::move(location)} {}

  constexpr bool empty() const { return m_empty; }

  template <typename Self>
  constexpr std::string_view category(this const Self& self) {
    using LogType = std::remove_cvref_t<Self>;
    if constexpr (std::derived_from<LogType, cpplogger::FatalContext>)
      return "fatal";
    else if constexpr (std::derived_from<LogType, cpplogger::ErrorContext>)
      return "error";
    else if constexpr (std::derived_from<LogType, cpplogger::WarningContext>)
      return "warn";
    else if constexpr (std::derived_from<LogType, cpplogger::InfoContext>)
      return "info";
    else if constexpr (std::derived_from<LogType, cpplogger::DebugContext>)
      return "debug";
    else if constexpr (std::derived_from<LogType, cpplogger::VerboseContext>)
      return "verbose";
    return "";
  }

 private:
  bool m_empty{false};
};

struct cpplogger::FatalContext : public cpplogger::BasicLogContext {};

struct cpplogger::ErrorContext : public cpplogger::BasicLogContext {};

struct cpplogger::WarningContext : public cpplogger::BasicLogContext {};

struct cpplogger::InfoContext : public cpplogger::BasicLogContext {};

struct cpplogger::DebugContext : public cpplogger::BasicLogContext {};

struct cpplogger::VerboseContext : public cpplogger::BasicLogContext {};

/**
 * @brief Format basic context information.
 */
template <typename T>
  requires std::derived_from<T, cpplogger::BasicLogContext>
struct std::formatter<T, char> {

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
  format(const T& logContext, FormatContext& fmtContext) const {
    auto it = fmtContext.out();
    if (std::string_view category = logContext.category(); !category.empty())
      it =
          std::format_to(it, "[{}]{}", category, logContext.empty() ? "" : " ");
    if (logContext.empty())
      return it;
    it = std::format_to(
        it,
        "{}: {}:{} `{}`",
        logContext.file_name(),
        logContext.line(),
        logContext.column(),
        logContext.function_name());
    return it;
  }
};

#endif
