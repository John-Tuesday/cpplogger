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

/**
 * @brief Context can be constructed using `std::source_location` and is
 * readable.
 */
template <typename T>
concept BasicLogContext = cpplogger::ReadableLogContext<T> &&
                          std::constructible_from<T, std::source_location>;

}  // namespace cpplogger

#endif
