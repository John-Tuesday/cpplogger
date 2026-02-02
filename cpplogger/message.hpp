#pragma once

#include <format>
#include <source_location>

namespace logger {

template <typename CharT, typename...>
struct LogFormatString;
template <typename CharT>
struct LogContext;
template <typename CharT>
struct FatalContext;
template <typename CharT>
struct ErrorContext;
template <typename CharT>
struct WarnContext;
template <typename CharT>
struct InfoContext;
template <typename CharT>
struct DebugContext;
template <typename CharT>
struct VerboseContext;
template <typename T>
constexpr bool isFatal(T&&);
template <typename T>
constexpr bool isError(T&&);
template <typename T>
constexpr bool isWarning(T&&);
template <typename T>
constexpr bool isInfo(T&&);
template <typename T>
constexpr bool isDebug(T&&);
template <typename T>
constexpr bool isVerbose(T&&);

/**
 * Base interface for logging context.
 *
 * Essentially a wrapper around `std::source_location` with an alias to the
 * `char` type for the log message.
 *
 * This type may be subclassed to add custom functionality to context's given to
 * various logger functions. Simply provide the type when invoking a log
 * function. Consumers can then distinguish different context types by way of
 * function overloads or template specialization.
 *
 * Custom logging context classes must be constructible from
 * `std::source_location` when used as the template argument of a call to log.
 */
template <typename CharT>
struct LogContext : public std::source_location {
  using CharType = std::remove_cvref_t<CharT>;

  constexpr static bool isFatal() { return false; }
  constexpr static bool isError() { return false; }
  constexpr static bool isWarning() { return false; }
  constexpr static bool isInfo() { return false; }
  constexpr static bool isDebug() { return false; }
  constexpr static bool isVerbose() { return false; }
};

/** Format string with extra information useful when logging. */
template <typename CharT, typename... Args>
struct LogFormatString
    : public std::basic_format_string<CharT, std::type_identity_t<Args>...> {

  /**
   * Forwards `fmt` to `std::format_string`.
   *
   * @note Generally, no input should be given for `location`; this way the
   * `std::source_location` will be constructed with respect to the callsite.
   *
   * @param[in] fmt forwarded as the first argument of the constructor for
   * `std::format_string<>`
   * @param[in] location location information to associated with this log
   * message
   */
  template <typename T>
  consteval LogFormatString(
      T&& fmt,
      std::source_location location = std::source_location::current())
      : std::basic_format_string<CharT, std::type_identity_t<Args>...>(
            std::forward<T>(fmt)),
        m_location(location) {}

  /**
   * Source location information associated with the format string.
   *
   * @return associated location
   */
  constexpr std::source_location location() const noexcept {
    return m_location;
  }

 private:
  std::source_location m_location{};
};

}  // namespace logger

template <typename CharT>
struct logger::FatalContext : public logger::LogContext<CharT> {
  constexpr static bool isFatal() { return true; }
};

template <typename CharT>
struct logger::ErrorContext : public logger::LogContext<CharT> {
  constexpr static bool isError() { return true; }
};

template <typename CharT>
struct logger::WarnContext : public logger::LogContext<CharT> {
  constexpr static bool isWarning() { return true; }
};

template <typename CharT>
struct logger::InfoContext : public logger::LogContext<CharT> {
  constexpr static bool isInfo() { return true; }
};

template <typename CharT>
struct logger::DebugContext : public logger::LogContext<CharT> {
  constexpr static bool isDebug() { return true; }
};

template <typename CharT>
struct logger::VerboseContext : public logger::LogContext<CharT> {
  constexpr static bool isVerbose() { return true; }
};

template <typename T>
constexpr bool logger::isFatal(T&& t) {
  if constexpr (requires() {
                  { t.isFatal() } -> std::same_as<bool>;
                }) {
    return t.isFatal();
  } else {
    return false;
  }
}

template <typename T>
constexpr bool logger::isError(T&& t) {
  if constexpr (requires() {
                  { t.isError() } -> std::same_as<bool>;
                }) {
    return t.isError();
  } else {
    return false;
  }
}

template <typename T>
constexpr bool logger::isWarning(T&& t) {
  if constexpr (requires() {
                  { t.isWarning() } -> std::same_as<bool>;
                })
    return t.isWarning();
  else
    return false;
}

template <typename T>
constexpr bool logger::isInfo(T&& t) {
  if constexpr (requires() {
                  { t.isInfo() } -> std::same_as<bool>;
                })
    return t.isInfo();
  else
    return false;
}

template <typename T>
constexpr bool logger::isDebug(T&& t) {
  if constexpr (requires() {
                  { t.isDebug() } -> std::same_as<bool>;
                })
    return t.isDebug();
  else
    return false;
}

template <typename T>
constexpr bool logger::isVerbose(T&& t) {
  if constexpr (requires() {
                  { t.isVerbose() } -> std::same_as<bool>;
                })
    return t.isVerbose();
  else
    return false;
}
