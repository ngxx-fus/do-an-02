idf_component_register(SRCS "myApp.c"
                    INCLUDE_DIRS ".")

target_include_directories(${COMPONENT_TARGET}
    PUBLIC
        ../config
        ../helper
)
