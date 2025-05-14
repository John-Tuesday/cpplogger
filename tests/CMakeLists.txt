add_executable(cpplogger_clitest)

target_sources(cpplogger_clitest
    PRIVATE
        clitest.cpp
)

target_compile_features(cpplogger_clitest
    PUBLIC
        cxx_std_23
)

target_link_libraries(cpplogger_clitest
    PRIVATE
        CppLogger::HeaderOnly
)

add_custom_target(run_cpplogger_clitest COMMAND cpplogger_clitest)
add_dependencies(run_cpplogger_clitest cpplogger_clitest)

add_test(NAME manual_test COMMAND cpplogger_clitest)

add_executable(cpplogger_sane_defaults_test)
target_sources(cpplogger_sane_defaults_test
    PRIVATE
        auto/sanedefaults.cpp
)
target_link_libraries(cpplogger_sane_defaults_test
    PRIVATE
        CppLogger::HeaderOnly
)
add_custom_target(run_cpplogger_sane_defaults COMMAND cpplogger_sane_defaults_test)
add_dependencies(run_cpplogger_sane_defaults cpplogger_sane_defaults_test)

add_custom_target(run_cpplogger_auto_tests COMMAND cpplogger_sane_defaults_test)
add_dependencies(run_cpplogger_auto_tests cpplogger_sane_defaults_test)
