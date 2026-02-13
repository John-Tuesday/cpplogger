#ifndef CPPLOGGER_LOGGER_HPP
#define CPPLOGGER_LOGGER_HPP

#include <format>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <utility>

namespace cpplogger {

template <typename CharT>
class BasicLogger;

struct DefaultTag;

template <typename>
struct DefaultLogger;

}  // namespace cpplogger

/**
 * @brief Tag used in combination with `cpplogger::DefaultLogger` to retreive
 * the default logger type.
 */
struct cpplogger::DefaultTag {};

/**
 * @brief Defines the default logger type; this may be specialized to inject a
 * custom default logger type.
 */
template <typename>
struct cpplogger::DefaultLogger {
  template <typename C>
  using Logger = cpplogger::BasicLogger<C>;
};

/**
 * @brief Base logger implementation with "compile-time polymorphism" for easier
 * subclassing.
 */
template <typename C = char>
class cpplogger::BasicLogger {
 public:
  /**
   * @brief Output character type.
   */
  using CharType = std::remove_cvref_t<C>;

  /**
   * @brief Format arguments and write to given output iterator.
   *
   * @see `std::format_to()`
   *
   * @return iterator past-the-end.
   */
  template <typename Out, typename Context, typename... Args>
    requires std::output_iterator<Out, const CharType&>
  Out formatTo(
      Out out,
      Context&& context,
      std::basic_format_string<CharType, std::type_identity_t<Args>...> fmt,
      Args&&... args) const {
    static_assert(
        std::formattable<Context, char>,
        "Context cannot be formatted to 'char'");
    static_assert(
        std::output_iterator<Out, const char&>,
        "Context cannot be written to the output iterator");
    auto it = std::format_to(
        std::forward<Out>(out),
        "{}",
        std::forward<Context>(context));
    if (std::formatted_size("{}", std::forward<Context>(context)) > 0) {
      *it = ' ';
      ++it;
    }
    it = std::format_to(it, fmt, std::forward<Args>(args)...);
    return it;
  }

  /**
   * @brief Format and write a log message to all output targets and appends a
   * new line.
   */
  template <typename Context, typename Self, typename... Args>
  void
  log(this Self&& self,
      Context&& context,
      std::basic_format_string<CharType, std::type_identity_t<Args>...> fmt,
      Args&&... args) {
    std::ostreambuf_iterator<CharType> out;
    if constexpr (std::same_as<char, CharType>) {
      out = std::cerr;
    } else if constexpr (std::same_as<wchar_t, CharType>) {
      out = std::wcerr;
    } else {
      std::unreachable();
    }
    out = std::forward<Self>(self).formatTo(
        out,
        std::forward<Context>(context),
        fmt,
        std::forward<Args>(args)...);
    out = std::format_to(out, "\n");
  }
};

#endif
