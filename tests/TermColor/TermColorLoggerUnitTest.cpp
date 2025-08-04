#include <CppLogger/TermColorLogger.hpp>

int main() {
  logger::TermColorLogger logger{};
  logger.log<logger::FatalContext<char>>("log message for {}", "Fatal");
  logger.log<logger::ErrorContext<char>>("log message for {}", "Error");
  logger.log<logger::WarnContext<char>>("log message for {}", "Warning");
  logger.log<logger::InfoContext<char>>("log message for {}", "Info");
  logger.log<logger::DebugContext<char>>("log message for {}", "Debug");
  logger.log<logger::VerboseContext<char>>("log message for {}", "Verbose");
  return 0;
}
