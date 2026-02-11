add_executable(cpplogger_manual)
set_target_properties(cpplogger_manual PROPERTIES
  OUTPUT_NAME "manual"
)

target_sources(cpplogger_manual
  PRIVATE
    clitest.cpp
)

target_compile_features(cpplogger_manual
  PUBLIC
    cxx_std_23
)

target_link_libraries(cpplogger_manual
  PRIVATE
    CppLogger::HeaderOnly
)

