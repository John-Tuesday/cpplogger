#include "shared.hpp"
#include "static.hpp"

int main() {
  cpplogger::minimal::staticTestLog();
  cpplogger::minimal::sharedTestLog();
  return 0;
}
