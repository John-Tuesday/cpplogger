#include "fixtures/result.hpp"
#include "fixtures/tempfiles.hpp"

#include <cpplogger/BasicLogger.hpp>
#include <cpplogger/LogContextFormatter.hpp>
#include <cpplogger/context.hpp>

#include <fstream>
#include <source_location>
#include <sstream>
#include <string_view>

namespace cpplogger::test {

class LogTargetsBasicFileLog;

template <typename Context>
cpplogger::test::Result testFileLog(
    std::string_view input,
    Context context = {std::source_location::current()});

}  // namespace cpplogger::test

int main() {
  if (auto result =
          cpplogger::test::testFileLog<cpplogger::InfoContext>("info: 5 == 5");
      !result)
    return cpplogger::test::printError(result.error());
  return 0;
}

class cpplogger::test::LogTargetsBasicFileLog
    : public cpplogger::BasicLogger<char> {

 public:
  LogTargetsBasicFileLog();

  template <typename Context, typename... Args>
  void
  log(Context&& context,
      std::format_string<std::type_identity_t<Args>...> fmt,
      Args&&... args) {
    std::ofstream logStream = getLogStream();
    std::ostreambuf_iterator fout{logStream};
    formatTo(
        fout,
        std::forward<Context>(context),
        fmt,
        std::forward<Args>(args)...);
    formatTo(
        std::ostreambuf_iterator{std::cerr},
        std::forward<Context>(context),
        fmt,
        std::forward<Args>(args)...);
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

cpplogger::test::LogTargetsBasicFileLog::LogTargetsBasicFileLog() {
  m_logPath = *cpplogger::test::tempDirectory() / getLogFileName();
}

std::ofstream cpplogger::test::LogTargetsBasicFileLog::getLogStream() const {
  return std::ofstream{getLogPath(), std::ios_base::out | std::ios_base::trunc};
}

/**
 * Log to a logger which writes to a file, then verify the contents of the file.
 */
template <typename Context>
cpplogger::test::Result
cpplogger::test::testFileLog(std::string_view input, Context context) {
  cpplogger::test::LogTargetsBasicFileLog logger{};
  std::stringstream captured{};
  logger.formatTo(std::ostreambuf_iterator{captured}, context, "{}", input);
  logger.log(context, "{}", input);
  std::string_view expect = captured.view();
  std::ifstream logIn{logger.getLogPath()};
  if (!logIn.is_open()) {
    return cpplogger::test::ResultFail(
        std::format(
            "Failed to open file '{}'",
            logger.getLogPath().generic_string()));
  }
  logIn.sync();
  std::string actual(expect.size(), '\0');
  logIn.read(actual.data(), actual.size());
  if (logIn.bad()) {
    return cpplogger::test::ResultFail(
        std::format(
            "I/O error while reading file '{}'",
            logger.getLogPath().generic_string()));
  }
  if (actual != expect) {
    return cpplogger::test::mismatch(expect, actual);
  }
  if (logIn.peek() != std::char_traits<char>::eof()) {
    return cpplogger::test::ResultFail("did not reach EOF");
  }
  return {};
}
