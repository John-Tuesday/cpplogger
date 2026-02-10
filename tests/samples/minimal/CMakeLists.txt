add_library(cpplogger_samples_minimal_shared SHARED)
target_sources(cpplogger_samples_minimal_shared
  PUBLIC FILE_SET HEADERS
  FILES
    shared.hpp
  PRIVATE
    shared.cpp
)
target_link_libraries(cpplogger_samples_minimal_shared
  PUBLIC
    CppLogger::HeaderOnly
)

add_library(cpplogger_samples_minimal_static STATIC)
target_sources(cpplogger_samples_minimal_static
  PUBLIC FILE_SET HEADERS
  FILES
    static.hpp
  PRIVATE
    static.cpp
)
target_link_libraries(cpplogger_samples_minimal_static
  PUBLIC
    CppLogger::HeaderOnly
)

add_executable(cpplogger_samples_minimal)
target_sources(cpplogger_samples_minimal
  PRIVATE
    main.cpp
)
target_link_libraries(cpplogger_samples_minimal
  PRIVATE
    cpplogger_samples_minimal_shared
    cpplogger_samples_minimal_static
)
add_test(NAME "CppLogger Minimal Sample"
  COMMAND cpplogger_samples_minimal
)

