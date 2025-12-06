idf_component_register(
    SRCS
        "LCD32.c"
    INCLUDE_DIRS
        "."
    REQUIRES
        AppConfig AppESPWrap AppUtils P16Com 
)