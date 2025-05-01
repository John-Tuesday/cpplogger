add_library(cpplogger_header_only INTERFACE)
add_library(CppLogger::HeaderOnly ALIAS cpplogger_header_only)

target_sources(cpplogger_header_only
    PUBLIC FILE_SET HEADERS
    FILES
        concepts.hpp
        logger.hpp
)

target_compile_features(cpplogger_header_only
    INTERFACE
        cxx_std_23
)

