#ifndef CPPLOGGER_CONCEPTS_HPP
#define CPPLOGGER_CONCEPTS_HPP

#include <concepts>
#include <source_location>

namespace cpplogger {

/**
 * @brief Context can be read from given type.
 */
template <typename T>
concept ReadableLogContext =
    std::convertible_to<T, const std::source_location&>;

}  // namespace cpplogger

#endif
