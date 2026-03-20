#include <cpplogger/log.hpp>

namespace cpplogger::test {

/**
 * Simply asserts all logging function compile.
 */
bool testLogFunctions(std::string_view msg);

}  // namespace cpplogger::test

int main() {
  if (!cpplogger::test::testLogFunctions("TEST")) {
    return 1;
  }
  return 0;
}

bool cpplogger::test::testLogFunctions(std::string_view msg) {
  // no context injection
  cpplogger::logFatal("message='{}'", msg);
  cpplogger::logError("message='{}'", msg);
  cpplogger::logError("message='{}'", msg);
  cpplogger::logWarning("message='{}'", msg);
  cpplogger::logInfo("message='{}'", msg);
  cpplogger::logDebug("message='{}'", msg);
  cpplogger::logVerbose("message='{}'", msg);
  // context injection
  constexpr std::source_location location;
  cpplogger::logFatal({"message='{}'", location}, msg);
  cpplogger::logError({"message='{}'", location}, msg);
  cpplogger::logError({"message='{}'", location}, msg);
  cpplogger::logWarning({"message='{}'", location}, msg);
  cpplogger::logInfo({"message='{}'", location}, msg);
  cpplogger::logDebug({"message='{}'", location}, msg);
  cpplogger::logVerbose({"message='{}'", location}, msg);
  return true;
}
