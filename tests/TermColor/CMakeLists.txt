add_executable(cpplogger_term_color_tests)

target_sources(cpplogger_term_color_tests
    PRIVATE
        TermColorLoggerUnitTest.cpp
)

target_link_libraries(cpplogger_term_color_tests
    PRIVATE
        CppLogger::TermColor
        TermColor::TermColor
)

add_custom_target(run_cpplogger_term_color_tests
    COMMAND cpplogger_term_color_tests
)

add_dependencies(run_cpplogger_term_color_tests cpplogger_term_color_tests)
