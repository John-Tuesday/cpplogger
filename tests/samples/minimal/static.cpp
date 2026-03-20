#include "static.hpp"

#include <cpplogger/LogContextFormatter.hpp>
#include <cpplogger/context.hpp>
#include <cpplogger/log.hpp>

void cpplogger::minimal::staticTestLog() {
  constexpr cpplogger::BasicLogContext context =
      std::source_location::current();
  cpplogger::log<cpplogger::BasicLogContext>(
      {"[static] minimal test log", context});
}
