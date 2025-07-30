cmake_minimum_required(VERSION 3.30)
project(CppLogger VERSION 0.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FetchContent)

option(cpplogger_fetch_term_color
    "Fetch TermColor package if it cannot be found"
    ON
)
if (NOT TARGET TermColor::TermColor AND "${cpplogger_fetch_term_color}")
    FetchContent_Declare(
        TuesdayTermColor
        GIT_REPOSITORY "https://github.com/John-Tuesday/term-color.git"
        GIT_TAG 5c1e0fdc8172ac817097bb65cf99f6c4b0c7b34b
        OVERRIDE_FIND_PACKAGE
    )
    find_package(TuesdayTermColor)
endif ()

add_subdirectory(src)
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
