#pragma once

#include <cassert>
#include <filesystem>
#include <iostream>
#include <print>

namespace logger::test {

inline std::filesystem::path tempDir() {
  std::error_code ec{};
  std::filesystem::path path =
      std::filesystem::temp_directory_path(ec) / "CppLogger";
  if (ec) {
    std::println(std::clog, "Error creating temp dir: '{}'", ec.message());
    assert(!ec);
    return std::filesystem::path{};
  }
  assert(!ec);
  return path;
}

} // namespace logger::test
