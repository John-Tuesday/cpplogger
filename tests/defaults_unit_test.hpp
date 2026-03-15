#ifndef TESTS_DEFAULTS_UNIT_TEST_HPP
#define TESTS_DEFAULTS_UNIT_TEST_HPP

#include <cpplogger/BasicLogger.hpp>
#include <cpplogger/Defaults.hpp>
#include <cpplogger/LogContextFormatter.hpp>
#include <cpplogger/context.hpp>

namespace cpplogger::test {

struct AltLogger : public cpplogger::BasicLogger<char> {
  template <typename Out, typename Context, typename... Args>
    requires std::output_iterator<Out, const CharType&>
  Out formatTo(
      Out out,
      Context&& context,
      std::basic_format_string<CharType, std::type_identity_t<Args>...> fmt,
      Args&&... args) const {
    auto it = std::format_to(std::forward<Out>(out), "[AltLogger]");
    return cpplogger::BasicLogger<char>::formatTo(
        it,
        std::forward<Context>(context),
        fmt,
        std::forward<Args>(args)...);
  }
};

}  // namespace cpplogger::test

template <>
struct cpplogger::Defaults<cpplogger::DefaultTag> {
  template <typename CharT>
  using Logger = cpplogger::test::AltLogger;
};

#include <cpplogger/log.hpp>

#endif
