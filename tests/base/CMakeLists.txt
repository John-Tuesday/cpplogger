add_subdirectory(Fixtures)
add_subdirectory(auto)

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
