add_executable(cpplogger_sane_defaults_test)
target_sources(cpplogger_sane_defaults_test
    PRIVATE
        sanedefaults.cpp
)
target_link_libraries(cpplogger_sane_defaults_test
    PRIVATE
        CppLogger::HeaderOnly
        CppLoggerTest::DefaultFixtures
)
add_custom_target(run_cpplogger_sane_defaults COMMAND cpplogger_sane_defaults_test)
add_dependencies(run_cpplogger_sane_defaults cpplogger_sane_defaults_test)

add_executable(cpplogger_test_file_log)
target_sources(cpplogger_test_file_log
    PUBLIC FILE_SET HEADERS
    FILES
        TestFileLog.hpp
    PRIVATE
        TestFileLog.cpp
)
target_link_libraries(cpplogger_test_file_log
    PRIVATE
        CppLogger::HeaderOnly
        CppLoggerTest::DefaultFixtures
)

add_custom_target(run_cpplogger_auto_tests
    COMMAND
        cpplogger_sane_defaults_test
        cpplogger_test_file_log
)
add_dependencies(run_cpplogger_auto_tests
    cpplogger_sane_defaults_test
    cpplogger_test_file_log
)
