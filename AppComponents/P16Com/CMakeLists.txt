idf_component_register(
    SRCS
        "P16Com.c"
    INCLUDE_DIRS
        "."
    REQUIRES 
        AppConfig AppESPWrap AppUtils P16Com
)