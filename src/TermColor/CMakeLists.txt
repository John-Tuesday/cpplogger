add_library(cpplogger_term_color_integration INTERFACE)
add_library(CppLogger::TermColor ALIAS cpplogger_term_color_integration)

target_sources(cpplogger_term_color_integration
    PUBLIC FILE_SET HEADERS
    BASE_DIRS
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INCLUDE_INSTALL_DIR}>"
    FILES
        CppLogger/TermColorLogger.hpp
)

target_link_libraries(cpplogger_term_color_integration
    INTERFACE
        CppLogger::HeaderOnly
        TermColor::TermColor
)
