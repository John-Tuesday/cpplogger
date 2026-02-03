add_subdirectory(Fixtures)
add_subdirectory(manual)

add_executable(cpplogger_sane_defaults)
target_sources(cpplogger_sane_defaults
  PRIVATE
    sanedefaults.cpp
)
target_link_libraries(cpplogger_sane_defaults
  PRIVATE
    CppLogger::HeaderOnly
    CppLoggerTest::DefaultFixtures
)

add_test(NAME cpplogger_sane_defaults_test
  COMMAND cpplogger_sane_defaults
)

add_executable(cpplogger_log_to_file)
target_sources(cpplogger_log_to_file
  PRIVATE
    TestFileLog.cpp
)
target_link_libraries(cpplogger_log_to_file
  PRIVATE
    CppLogger::HeaderOnly
    CppLoggerTest::DefaultFixtures
)

add_test(NAME cpplogger_log_to_file_test
  COMMAND cpplogger_log_to_file
)

