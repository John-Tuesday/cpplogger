add_library(cpplogger_header_only INTERFACE)
add_library(CppLogger::HeaderOnly ALIAS cpplogger_header_only)

target_sources(cpplogger_header_only
    PUBLIC FILE_SET HEADERS
    BASE_DIRS
        "$<BUILD_INTERFACE:$<PATH:GET_PARENT_PATH,${CMAKE_CURRENT_SOURCE_DIR}>>"
        "$<INSTALL_INTERFACE:${CMAKE_INCLUDE_INSTALL_DIR}>"
    FILES
        concepts.hpp
        logger.hpp
)

target_compile_features(cpplogger_header_only
    INTERFACE
        cxx_std_23
)

