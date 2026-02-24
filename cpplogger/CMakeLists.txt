add_library(cpplogger_header_only INTERFACE)
add_library(CppLogger::HeaderOnly ALIAS cpplogger_header_only)

target_sources(cpplogger_header_only
  PUBLIC FILE_SET HEADERS
  BASE_DIRS
    "$<BUILD_INTERFACE:${CppLogger_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INCLUDE_INSTALL_DIR}>"
  FILES
    concepts.hpp
    log.hpp
    logger.hpp
    context.hpp
    BasicLogger.hpp
    LogContextFormatString.hpp
    LogContextFormatter.hpp
)

target_compile_features(cpplogger_header_only
  INTERFACE
    cxx_std_23
)

set(cpplogger_generated_dir "${CppLogger_BINARY_DIR}/generated")
block(SCOPE_FOR VARIABLES PROPAGATE cpplogger_header_common_path)
  set(header_target "CppLogger::HeaderOnly")
  set(base_include_dir "$<TARGET_PROPERTY:${header_target},HEADER_DIRS>")
  set(base_include_dir "$<TARGET_GENEX_EVAL:${header_target},${base_include_dir}>")
  set(base_include_dir "$<LIST:FILTER,${base_include_dir},INCLUDE,.+>")
  set(base_include_dir "$<LIST:GET,${base_include_dir},0>")

  set(header_paths "$<TARGET_PROPERTY:${header_target},HEADER_SET>")
  set(header_paths "$<TARGET_GENEX_EVAL:${header_target},${header_paths}>")
  set(header_paths "$<PATH:RELATIVE_PATH,${header_paths},${base_include_dir}>")

  set(header_includes "$<LIST:TRANSFORM,${header_paths},REPLACE,(^.+$),#include <\\1$<ANGLE-R>>")
  set(header_includes "$<LIST:JOIN,${header_includes},\n>")

  set(cpplogger_header_common_path "${cpplogger_generated_dir}/CppLogger")
  file(GENERATE OUTPUT "${cpplogger_header_common_path}"
    CONTENT "\
\#ifndef CPPLOGGER_CPPLOGGER
\#define CPPLOGGER_CPPLOGGER

${header_includes}

\#endif
"
  )
  set_source_files_properties("${cpplogger_header_common_path}" PROPERTIES
    GENERATED ON
  )
  add_custom_target(cpplogger_header_common_generate DEPENDS "${cpplogger_header_common_path}")
endblock()

target_sources(cpplogger_header_only
  PUBLIC FILE_SET common TYPE HEADERS
  BASE_DIRS
    "$<BUILD_INTERFACE:${cpplogger_generated_dir}>"
    # TODO: Verify this makes sense and doesn't conflict with the normal headers.
    "$<INSTALL_INTERFACE:${CMAKE_INCLUDE_INSTALL_DIR}>"
  FILES
    "${cpplogger_header_common_path}"
)
