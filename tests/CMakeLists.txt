add_executable(clitest)

target_sources(clitest
    PUBLIC
        clitest.cpp
)

target_compile_features(clitest
    PUBLIC
        cxx_std_23
)

target_link_libraries(clitest
    PUBLIC
        header_only
)

add_test(NAME manual_test COMMAND clitest)
