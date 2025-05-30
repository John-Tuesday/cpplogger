#pragma once

#include <iostream>
#include <ranges>
#include <source_location>
#include <utility>

namespace logger {
enum class MessageType;
}

/**
 * @brief Constraints for logging helper classes.
 */
namespace logger::concepts {

/**
 * Output device used when writing logs
 */
template <typename T>
concept LogTarget = std::constructible_from<std::osyncstream, T>;

/**
 * Tuple-Like type whose elements all satisfy `LogTarget`.
 */
template <typename T>
concept TupleLikeOfLogTargets = requires {
  std::apply([]<LogTarget... Args>(Args &&...) constexpr {}, std::declval<T>());
};

/**
 * Provides a collection of output targets in response to logging context.
 *
 * @tparam MType type of logging message which will be written.
 */
template <typename T, MessageType MType>
concept ProvidesLogOutputTargets = requires(T t) {
  {
    t.template targets<MType>(std::declval<const std::source_location &>())
  } -> TupleLikeOfLogTargets;
};

/** Callable that returns a usable output stream. */
template <typename T>
concept IndirectlyProvidesLogTarget = LogTarget<std::indirect_result_t<T>>;

/**
 * Tuple-like struct of log target providers.
 */
template <typename T>
concept TupleLikeOfIndirectLogTargets = requires(T t) {
  {
    std::apply([]<typename... Args>
                 requires(IndirectlyProvidesLogTarget<Args> && ...)
               (Args &&...) constexpr { return; },
               t)
  };
};

/** Range of objects with constraint `ProvidesLogTarget`. */
template <typename T>
concept RangeOfIndirectLogTargets =
    std::ranges::range<T> &&
    IndirectlyProvidesLogTarget<std::ranges::range_value_t<T>>;

/** Provides a range of log target provides. */
template <typename T, MessageType M>
concept ProvidesIndirectLogTargets = requires(T t) {
  { t.template providers<M>() } noexcept;
  requires requires(decltype(t.template providers<M>()) r) {
    requires(RangeOfIndirectLogTargets<decltype(r)> ||
             TupleLikeOfIndirectLogTargets<decltype(r)>);
  };
};

/**
 * Provides a function to print a log messages.
 */
template <typename T, MessageType MType>
concept PrintsToLog = requires(T t) {
  {
    t.template print<MType>(std::declval<std::ostream &>(),
                            std::declval<std::source_location>(), "message")
  };
};

/** Meets the requirements of a log filter. */
template <typename T, MessageType MType>
concept FiltersLog = requires(T t) {
  {
    t.template filter<MType>(std::declval<std::source_location>())
  } noexcept -> std::same_as<bool>;
};

} // namespace logger::concepts
