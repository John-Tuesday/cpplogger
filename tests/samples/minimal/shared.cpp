#include "shared.hpp"

#include <cpplogger/LogContextFormatter.hpp>
#include <cpplogger/context.hpp>
#include <cpplogger/log.hpp>

void cpplogger::minimal::sharedTestLog() {
  cpplogger::BasicLogContext context = std::source_location::current();
  cpplogger::log(context, "[shared] minimal test log");
}
