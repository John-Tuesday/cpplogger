add_subdirectory(fixtures)

add_executable(cpplogger_logger_unit_test)
target_sources(cpplogger_logger_unit_test
  PRIVATE
    logger_unit_test.cpp
)
target_link_libraries(cpplogger_logger_unit_test
  PRIVATE
    CppLogger::HeaderOnly
)
add_test(NAME "CppLogger: Logger [Unit Test]"
  COMMAND cpplogger_logger_unit_test
)

add_executable(cpplogger_logcontext_unit_test)
target_sources(cpplogger_logcontext_unit_test
  PRIVATE
    logcontext_formatter_unit_test.cpp
)
target_link_libraries(cpplogger_logcontext_unit_test
  PRIVATE
    CppLogger::HeaderOnly
)
add_test(NAME "CppLogger: LogContext [Unit Test]"
  COMMAND cpplogger_logcontext_unit_test
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

add_subdirectory(manual)
add_subdirectory(samples)

