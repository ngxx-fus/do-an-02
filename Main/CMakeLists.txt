idf_component_register(
    SRCS "Application.c"
    INCLUDE_DIRS "."
)

target_include_directories(${COMPONENT_TARGET}
    PUBLIC
        ../AppConfig
        ../AppComponents
        ../AppUtils
)
