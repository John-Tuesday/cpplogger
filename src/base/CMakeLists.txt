add_library(cpplogger_header_only INTERFACE)
add_library(CppLogger::HeaderOnly ALIAS cpplogger_header_only)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH build_base_dir)

target_sources(cpplogger_header_only
    PUBLIC FILE_SET HEADERS
    BASE_DIRS
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INCLUDE_INSTALL_DIR}>"
    FILES
        CppLogger/concepts.hpp
        CppLogger/logger.hpp
        CppLogger/message.hpp
)

target_compile_features(cpplogger_header_only
    INTERFACE
        cxx_std_23
)

