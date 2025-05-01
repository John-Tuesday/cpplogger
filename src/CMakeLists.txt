add_library(header_only INTERFACE)

target_sources(header_only
    PUBLIC FILE_SET HEADERS
    FILES
        concepts.hpp
        logger.hpp
)

target_compile_features(header_only
    INTERFACE
        cxx_std_23
)

