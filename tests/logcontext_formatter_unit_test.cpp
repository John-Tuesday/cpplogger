#include "fixtures/result.hpp"

#include <cpplogger/LogContextFormatter.hpp>
#include <cpplogger/context.hpp>

#include <expected>
#include <format>
#include <sstream>

namespace cpplogger::test {
namespace {

static_assert(std::formattable<cpplogger::BasicLogContext, char>);
static_assert(std::formattable<cpplogger::FatalContext, char>);
static_assert(std::formattable<cpplogger::ErrorContext, char>);
static_assert(std::formattable<cpplogger::WarningContext, char>);
static_assert(std::formattable<cpplogger::InfoContext, char>);
static_assert(std::formattable<cpplogger::DebugContext, char>);
static_assert(std::formattable<cpplogger::VerboseContext, char>);

template <typename T>
struct DerivedContext : public T {};

template <typename T>
cpplogger::test::Result derivedMatchesCategoryName() {
  T expect;
  DerivedContext<T> actual;
  if (expect.category() != actual.category())
    return cpplogger::test::mismatch(expect.category(), actual.category());
  return {};
}

template <typename... Ts>
cpplogger::test::Result derivedContextKeepsCategoryName() {
  std::array results = {derivedMatchesCategoryName<Ts>()...};
  std::stringstream stream{};
  std::ostreambuf_iterator out{stream};
  bool pass = true;
  for (auto& res : results) {
    if (!res) {
      stream << res.error();
      if (!pass)
        stream << "\n";
      else
        pass = false;
    }
  }
  if (!pass)
    return cpplogger::test::ResultFail{std::move(stream.str())};
  return {};
}

template <typename T>
cpplogger::test::Result formatEmptyKeepsOnlyCategory(T context) {
  assert(context.empty());
  std::string actual = std::format("{}", context);
  std::string expect =
      context.category().empty() ? "" : std::format("[{}]", context.category());
  if (actual != expect)
    return cpplogger::test::mismatch(expect, actual);
  return {};
}

template <typename... Ts>
cpplogger::test::Result testAllContexts() {
  if (auto result = derivedContextKeepsCategoryName<Ts...>(); !result)
    return result;
  return {};
}

}  // namespace
}  // namespace cpplogger::test

int main() {
  if (auto result = cpplogger::test::testAllContexts<
          cpplogger::BasicLogContext,
          cpplogger::FatalContext,
          cpplogger::ErrorContext,
          cpplogger::WarningContext,
          cpplogger::InfoContext,
          cpplogger::DebugContext,
          cpplogger::VerboseContext>();
      !result)
    return cpplogger::test::printError(result.error());
  return 0;
}
