#ifndef CPPLOGGER_CONCEPTS_HPP
#define CPPLOGGER_CONCEPTS_HPP

#include <concepts>
#include <source_location>

namespace cpplogger {

/**
 * @brief Context can be constructed using `std::source_location` and is
 * readable.
 */
template <typename T>
concept BasicLogContext = std::constructible_from<T, std::source_location>;

}  // namespace cpplogger

#endif
