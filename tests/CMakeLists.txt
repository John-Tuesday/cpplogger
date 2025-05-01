add_executable(cpplogger_clitest)

target_sources(cpplogger_clitest
    PUBLIC
        clitest.cpp
)

target_compile_features(cpplogger_clitest
    PUBLIC
        cxx_std_23
)

target_link_libraries(cpplogger_clitest
    PUBLIC
        cpplogger_header_only
)

add_test(NAME manual_test COMMAND cpplogger_clitest)
