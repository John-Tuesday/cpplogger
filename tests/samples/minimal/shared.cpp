#include "shared.hpp"

#include <cpplogger/context.hpp>
#include <cpplogger/log.hpp>

void cpplogger::minimal::sharedTestLog() {
  cpplogger::LogContext context = std::source_location::current();
  cpplogger::log(context, "[shared] minimal test log");
}
