#ifndef CPPLOGGER_DEFAULTLOGGER_HPP
#define CPPLOGGER_DEFAULTLOGGER_HPP

#include "BasicLogger.hpp"

namespace cpplogger {

struct DefaultTag;

template <typename T>
struct Defaults;

}  // namespace cpplogger

/**
 * @brief Tag which specifies the requrest for the default logger type.
 */
struct cpplogger::DefaultTag {};

/**
 * @brief Defines the default types; this may be specialized to inject custom
 * types.
 */
template <typename T>
struct cpplogger::Defaults {
  template <typename CharT>
  using Logger = cpplogger::BasicLogger<CharT>;
};

#endif
