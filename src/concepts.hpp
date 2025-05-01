#pragma once

#include <iostream>
#include <ranges>
#include <source_location>
#include <string_view>

namespace logger {
enum class MessageType;
}

namespace logger::concepts {

template <typename T>
concept ProvidesLogTarget = requires(T t) {
  { t() } -> std::common_reference_with<std::ostream &>;
};

template <typename T>
concept LogTargetProviderRange =
    std::ranges::range<T> && ProvidesLogTarget<std::ranges::range_value_t<T>>;

template <typename T, MessageType M>
concept IndirectlyProvidesLogTargets = requires(T t) {
  { t.template logproviders<M>() } noexcept -> LogTargetProviderRange;
};

template <typename T, MessageType M>
concept IsLogFormatter = requires(T t) {
  {
    t.template format<M>(std::declval<std::source_location>(),
                         std::ostream_iterator<char>(std::cout),
                         std::declval<std::string_view>())
  } -> std::same_as<decltype(std::ostream_iterator<char>(std::cout))>;
};

template <typename T, MessageType M>
concept IsLogFilter = requires(T t) {
  {
    t.template filter<M>(std::declval<std::source_location>())
  } noexcept -> std::same_as<bool>;
};

} // namespace logger::concepts
