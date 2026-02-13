#include <cpplogger/context.hpp>

#include <expected>
#include <format>
#include <print>
#include <sstream>

namespace cpplogger::test {
namespace {

static_assert(std::formattable<cpplogger::FatalContext, char>);
static_assert(std::formattable<cpplogger::ErrorContext, char>);
static_assert(std::formattable<cpplogger::WarningContext, char>);
static_assert(std::formattable<cpplogger::InfoContext, char>);
static_assert(std::formattable<cpplogger::DebugContext, char>);
static_assert(std::formattable<cpplogger::VerboseContext, char>);

template <typename T>
struct DerivedContext : public T {};

template <typename T>
std::expected<void, std::string> derivedMatchesCategoryName() {
  T expect;
  DerivedContext<T> actual;
  if (expect.category() != actual.category())
    return std::unexpected(
        std::format(
            "expected: {}\n"
            "actual: {}",
            expect.category(),
            actual.category()));
  return {};
}

template <typename... Ts>
std::expected<void, std::string> derivedContextKeepsCategoryName() {
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
    return std::unexpected{std::move(stream.str())};
  return {};
}

template <typename... Ts>
std::expected<void, std::string> testAllContexts() {
  if (std::expected result = derivedContextKeepsCategoryName<Ts...>(); !result)
    return result;
  return {};
}

}  // namespace
}  // namespace cpplogger::test

int main() {
  if (auto result = cpplogger::test::testAllContexts<
          cpplogger::FatalContext,
          cpplogger::ErrorContext,
          cpplogger::WarningContext,
          cpplogger::InfoContext,
          cpplogger::DebugContext,
          cpplogger::VerboseContext>();
      !result) {
    std::println("failed\n{}", result.error());
    return 1;
  }
  return 0;
}
