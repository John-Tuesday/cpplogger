#ifndef CPPLOGGER_LOGGER_HPP
#define CPPLOGGER_LOGGER_HPP

#include "BasicLogger.hpp"

namespace cpplogger {

struct DefaultTag;

template <typename>
struct DefaultLogger;

}  // namespace cpplogger

/**
 * @brief Tag used in combination with `cpplogger::DefaultLogger` to retreive
 * the default logger type.
 */
struct cpplogger::DefaultTag {};

/**
 * @brief Defines the default logger type; this may be specialized to inject a
 * custom default logger type.
 */
template <typename>
struct cpplogger::DefaultLogger {
  template <typename C>
  using Logger = cpplogger::BasicLogger<C>;
};

#endif
