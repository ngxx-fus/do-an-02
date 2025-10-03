idf_component_register(SRCS "do-an-02.c"
                    INCLUDE_DIRS ".")

target_include_directories(${COMPONENT_TARGET}
    PUBLIC
        ../config
        ../helper
)
