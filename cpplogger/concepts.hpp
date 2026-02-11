#ifndef CPPLOGGER_CONCEPTS_HPP
#define CPPLOGGER_CONCEPTS_HPP

#include <concepts>
#include <format>
#include <source_location>
#include <type_traits>

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

/**
 * @brief Categorized context.
 */
template <typename T, typename CharT>
concept CategorizedLogContext =
    cpplogger::ReadableLogContext<T> &&
    std::formattable<decltype(std::remove_cvref_t<T>::category), CharT>;

}  // namespace cpplogger

#endif
