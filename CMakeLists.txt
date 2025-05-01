cmake_minimum_required(VERSION 3.30)
project(format_logger VERSION 0.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_subdirectory(src/CppLogger)
add_subdirectory(tests)
