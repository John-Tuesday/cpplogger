#pragma once

#ifndef CPPLOGGER_TERM_COLOR_LOGGER
#define CPPLOGGER_TERM_COLOR_LOGGER

#ifdef __has_include
#if __has_include(<TermColor>)
#else
#error "Cannnot include <TermColor>. Did you forget to link TermColor library"
#endif
#endif

#include <TermColor>

#include <CppLogger/logger.hpp>

namespace logger {

struct TermColorLogger : public LoggerDefaults<logger::DefaultImplTag> {
  template <logger::concepts::LogContextLike Context>
  constexpr std::string_view getColors(Context &&context) {
    if constexpr (logger::isFatal(context) || logger::isError(context)) {
      return sgr::Red::Foreground::str();
    }
    if constexpr (logger::isWarning(context)) {
      return sgr::Yellow::Foreground::str();
    }
    return sgr::reset.str();
  }

  template <
      logger::concepts::LogContextLike Context,
      logger::concepts::PrintableStream<logger::LogContextCharType<Context>>
          Stream>
  void
  print(Stream &&stream, const Context &context,
        std::basic_string_view<logger::LogContextCharType<Context>> message) {
    if (sgr::isTerminalOutputStream(stream)) {
      auto color = getColors(context);
      std::print(stream, "{}{}", color, sgr::bold);
      LoggerDefaults::print(
          stream, context,
          std::format("{}{}{}{}", sgr::reset, color, message, sgr::bold));
      std::print(stream, "{}", sgr::reset);
    } else {
      LoggerDefaults::print(stream, context, message);
    }
  }
};

} // namespace logger

#endif
