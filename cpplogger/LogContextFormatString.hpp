#ifndef CPPLOGGER_LOGCONTEXTFORMATSTRING_HPP
#define CPPLOGGER_LOGCONTEXTFORMATSTRING_HPP

#include <format>
#include <source_location>

namespace cpplogger {

template <typename Context, typename CharT, typename... Args>
  requires std::constructible_from<Context, std::source_location>
class LogContextFormatString;

}

/**
 * @brief Intended to be used a drop-in replacement for
 * `std::basic_format_string<>` but with extra context information for logging
 * purposes.
 */
template <typename Ctx, typename CharT, typename... Args>
  requires std::constructible_from<Ctx, std::source_location>
class cpplogger::LogContextFormatString
    : public std::basic_format_string<CharT, std::type_identity_t<Args>...> {
 public:
  /**
   * @brief Type which is stored by `this` and should provide contextual
   * information to loggers.
   */
  using Context = Ctx;

  /**
   * @brief Character type used to represent the underlying format string.
   */
  using CharType = CharT;

  /**
   * @brief Intended to be used a drop-in replacement for
   * `std::basic_format_string<>`.
   *
   * Forwards `fmt` to `std::basic_format_string<>`.
   *
   * @note The default value for `location` will describe the callsite.
   *
   * @param[in] fmt forwarded as the first argument of the constructor for
   * `std::basic_format_string<>`
   * @param[in] location used to initialize the associated context variable
   */
  template <typename T>
  consteval LogContextFormatString(
      T&& fmt,
      std::source_location location = std::source_location::current())
      : LogContextFormatString(std::forward<T>(fmt), Context{location}) {}

  constexpr LogContextFormatString(LogContextFormatString&&) = default;

  template <typename T>
  consteval LogContextFormatString(T&& fmt, Context ctx)
      : std::basic_format_string<
            CharT,
            std::type_identity_t<Args>...>{std::forward<T>(fmt)},
        m_context{ctx} {}

  /**
   * @return const reference to associated context.
   */
  constexpr const Context& context() const { return m_context; }

 private:
  Context m_context{};
};

#endif
