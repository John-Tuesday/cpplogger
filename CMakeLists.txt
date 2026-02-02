cmake_minimum_required(VERSION 3.30)
project(CppLogger VERSION 0.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FetchContent)

add_subdirectory(cpplogger)
add_subdirectory(tests)

find_package(Doxygen)
if(${DOXYGEN_FOUND})
  block(SCOPE_FOR VARIABLES)
    set(DOXYGEN_PROJECT_NUMBER "${CppLogger_VERSION}$<$<NOT:$<CONFIG:Release>>:-dev>")
    set(DOXYGEN_OUTPUT_DIRECTORY "${CppLogger_BINARY_DIR}/docs/api/v${DOXYGEN_PROJECT_NUMBER}")
    set(DOXYGEN_WARN_LOGFILE "${CppLogger_BINARY_DIR}/doxygen-warning.log")
    set(envars "")
    list(APPEND envars 
      "DOXYGEN_PROJECT_NAME=${CppLogger}"
      "DOXYGEN_PROJECT_NUMBER=${DOXYGEN_PROJECT_NUMBER}"
      "DOXYGEN_OUTPUT_DIRECTORY=${DOXYGEN_OUTPUT_DIRECTORY}"
      "DOXYGEN_WARN_LOGFILE=${DOXYGEN_WARN_LOGFILE}"
    )
    get_target_property(in_files CppLogger::HeaderOnly HEADER_SET)
    list(JOIN in_files " " in_files)
    list(APPEND envars "DOXYGEN_INPUT=${in_files}")
    add_custom_target(cpplogger_generate_docs
      COMMAND cmake -E rm -r -f "${DOXYGEN_OUTPUT_DIRECTORY}"
      COMMAND cmake -E make_directory "${DOXYGEN_OUTPUT_DIRECTORY}"
      COMMAND cmake -E env ${envars} -- doxygen
      COMMAND cmake -E cat "${DOXYGEN_WARN_LOGFILE}"
      WORKING_DIRECTORY "${CppLogger_SOURCE_DIR}"
      BYPRODUCTS "${DOXYGEN_OUTPUT_DIRECTORY}"
      VERBATIM
    )
  endblock()
endif()
