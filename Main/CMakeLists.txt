idf_component_register(
    SRCS 
        "Application.c"
    INCLUDE_DIRS 
        "."
    REQUIRES 
        esp_driver_gpio esp_driver_spi esp_timer
        AppConfig AppESPWrap AppUtils AppFonts P16Com LCD32
)

target_include_directories(${COMPONENT_TARGET}
    PUBLIC
    ../AppCore
    ../AppComponents
    ../AppESPWrap
    ../AppFonts
    ../AppUtils
    ../AppConfig
)
