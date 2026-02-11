add_executable(cpplogger_manual)
set_target_properties(cpplogger_manual PROPERTIES
  OUTPUT_NAME "manual"
)
target_sources(cpplogger_manual
  PRIVATE
    main.cpp
)
target_link_libraries(cpplogger_manual
  PRIVATE
    CppLogger::HeaderOnly
)

