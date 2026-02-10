#include "static.hpp"

#include <cpplogger/context.hpp>
#include <cpplogger/log.hpp>

void cpplogger::minimal::staticTestLog() {
  cpplogger::LogContext context = std::source_location::current();
  cpplogger::log(context, "[static] minimal test log");
}
