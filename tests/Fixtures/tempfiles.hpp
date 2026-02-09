#pragma once

#ifndef TESTS_FIXTURES_TEMPFILES_HPP
#define TESTS_FIXTURES_TEMPFILES_HPP

#include <expected>
#include <filesystem>
#include <string_view>

namespace cpplogger::test {

inline constexpr std::string_view tempDirectoryName = "CppLogger";

/**
 * Return the temperary directory for use by tests.
 *
 * If the directory does not exist, it will be created.
 *
 * @return the directory path or the coresponding `std::error_code`.
 */
static std::expected<std::filesystem::path, std::error_code> tempDirectory();

}  // namespace cpplogger::test

std::expected<std::filesystem::path, std::error_code>
cpplogger::test::tempDirectory() {
  std::error_code ec{};
  std::filesystem::path path = std::filesystem::temp_directory_path(ec) /
                               cpplogger::test::tempDirectoryName;
  if (ec)
    return std::unexpected{ec};
  ec.clear();
  std::filesystem::create_directory(path, ec);
  if (ec)
    return std::unexpected{ec};
  return path;
}

#endif
