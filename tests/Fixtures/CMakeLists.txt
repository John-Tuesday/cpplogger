add_library(cpplogger_default_fixtures INTERFACE)
add_library("CppLoggerTest::DefaultFixtures" ALIAS cpplogger_default_fixtures)

target_sources(cpplogger_default_fixtures
    PUBLIC FILE_SET HEADERS
    BASE_DIRS
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>"
    FILES
        DefaultImpl.hpp
)

target_link_libraries(cpplogger_default_fixtures
    INTERFACE
        CppLogger::HeaderOnly
)

