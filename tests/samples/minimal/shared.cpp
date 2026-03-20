#include "shared.hpp"

#include <cpplogger/LogContextFormatter.hpp>
#include <cpplogger/context.hpp>
#include <cpplogger/log.hpp>

void cpplogger::minimal::sharedTestLog() {
  constexpr cpplogger::BasicLogContext context =
      std::source_location::current();
  cpplogger::log<cpplogger::BasicLogContext>(
      {"[shared] minimal test log", context});
}
