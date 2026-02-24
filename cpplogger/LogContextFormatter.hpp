#ifndef CPPLOGGER_LOGCONTEXTFORMATTER_HPP
#define CPPLOGGER_LOGCONTEXTFORMATTER_HPP

#include "context.hpp"

#include <format>

template <typename T>
  requires std::derived_from<T, cpplogger::BasicLogContext>
struct std::formatter<T, char>;

namespace cpplogger {

/**
 * @brief Alias for a log context formatter
 */
template <typename T, typename CharT = char>
using LogContextFormatter = std::formatter<T, CharT>;

}  // namespace cpplogger

/**
 * @brief Format basic context information.
 */
template <typename T>
  requires std::derived_from<T, cpplogger::BasicLogContext>
struct std::formatter<T, char> {

  /**
   * @brief Parse format-spec.
   *
   * No formatting specification is provided or implemented. An error is given
   * if anything is format-spec is given.
   */
  template <typename ParseContext>
  constexpr ParseContext::iterator parse(ParseContext& context) {
    auto it = context.begin();
    if (it != context.end() && *it != '}') {
      throw std::format_error("Unrecognized format args");
    }
    return it;
  }

  /**
   * @brief Write formatted text to output.
   */
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
