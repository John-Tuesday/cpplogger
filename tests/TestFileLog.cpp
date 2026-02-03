#include "Fixtures/tempfiles.hpp"

#include <cpplogger/logger.hpp>

#include <cassert>
#include <fstream>
#include <source_location>
#include <sstream>
#include <string_view>
#include <tuple>

namespace logger::test {

class LogTargetsBasicFileLog;

class LogTargetsBasicFileLog : public logger::LoggerDefaults<void> {

 public:
  LogTargetsBasicFileLog();

  template <logger::concepts::LogContextLike Context>
  auto targets(const Context& location) const noexcept
      -> logger::concepts::TupleLikeOfLogTargets<
          LogContextCharType<Context>> decltype(auto) {
    return std::tuple(std::ref(std::cerr), getLogStream());
  }

  constexpr std::string_view getLogFileName() const {
    return "basicfiletest.log";
  }

  const std::filesystem::path& getLogPath() const { return m_logPath; }

 private:
  std::filesystem::path m_logPath{};

  /**
   * Opens log file for ouput.
   *
   * The file is created if it does not exist and the contents of the file are
   * cleared if it does.
   */
  std::ofstream getLogStream() const;
};

bool testFileLog(std::string_view input);

}  // namespace logger::test

int main() {
  if (!logger::test::testFileLog("info: 5 == 5"))
    return 1;
  return 0;
}

logger::test::LogTargetsBasicFileLog::LogTargetsBasicFileLog() {
  m_logPath = *logger::test::tempDirectory() / getLogFileName();
}

std::ofstream logger::test::LogTargetsBasicFileLog::getLogStream() const {
  return std::ofstream{getLogPath(), std::ios_base::out | std::ios_base::trunc};
}

/**
 * Log to a logger which writes to a file, then verify the contents of the file.
 *
 * TODO: Find a better to communicate reasons for failure
 * TODO: Communicate when failure is due to file access, like log file is cannot
 * be written to.
 */
bool logger::test::testFileLog(std::string_view input) {
  using Context = logger::InfoContext<char>;
  logger::test::LogTargetsBasicFileLog logger{};
  std::stringstream captured{};
  Context context{std::source_location::current()};
  logger.print(captured, context, input);
  logger.write(context, input);
  std::string_view expect = captured.view();
  std::ifstream logIn{logger.getLogPath()};
  logIn.sync();
  std::string actual(expect.size(), '\0');
  logIn.read(actual.data(), actual.size());
  if (actual != expect) {
    std::println(
        "log file did not contain expected contents\n"
        "  file: '{}'\n"
        "  expected: '{}'\n  actual: '{}'",
        logger.getLogPath().generic_string(),
        expect,
        actual);
    return false;
  }
  if (logIn.peek() != std::char_traits<char>::eof()) {
    std::println("did not reach EOF");
    return false;
  }
  return true;
}
