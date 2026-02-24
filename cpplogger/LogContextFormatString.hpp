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
 *
 * @todo Consider letting the context variable be mutable.
 * @todo The context getter should reflect the mutability of the context.
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
