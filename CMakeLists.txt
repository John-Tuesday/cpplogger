cmake_minimum_required(VERSION 3.30)
project(CppLogger VERSION 0.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_subdirectory(src/CppLogger)
add_subdirectory(tests)

find_package(Doxygen)
if(${DOXYGEN_FOUND})
    block(SCOPE_FOR VARIABLES)
        set(DOXYGEN_PROJECT_NUMBER "${PROJECT_VERSION}$<$<NOT:$<CONFIG:Release>>:-dev>")
        set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/docs/api/v${DOXYGEN_PROJECT_NUMBER}")
        list(APPEND envars "DOXYGEN_PROJECT_NAME=${PROJECT_NAME}")
        list(APPEND envars "DOXYGEN_PROJECT_NUMBER=${DOXYGEN_PROJECT_NUMBER}")
        list(APPEND envars "DOXYGEN_OUTPUT_DIRECTORY=${DOXYGEN_OUTPUT_DIRECTORY}")
        get_target_property(in_files CppLogger::HeaderOnly HEADER_SET)
        list(JOIN in_files " " in_files)
        list(APPEND envars "DOXYGEN_INPUT=${in_files}")
        set(DOXYGEN_WARN_LOGFILE "${CMAKE_BINARY_DIR}/doxygen-warning.log")
        list(APPEND envars "DOXYGEN_WARN_LOGFILE=${DOXYGEN_WARN_LOGFILE}")
        add_custom_target(generate_docs
            COMMAND cmake -E rm -r -f "${DOXYGEN_OUTPUT_DIRECTORY}"
            COMMAND cmake -E make_directory "${DOXYGEN_OUTPUT_DIRECTORY}"
            COMMAND cmake -E env ${envars} -- doxygen
            COMMAND cmake -E cat "${DOXYGEN_WARN_LOGFILE}"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            BYPRODUCTS "${DOXYGEN_OUTPUT_DIRECTORY}"
            VERBATIM
        )
    endblock()
endif()
