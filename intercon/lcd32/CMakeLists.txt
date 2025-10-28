idf_component_register(
    SRCS "lcd32.c"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_gpio esp_driver_spi esp_timer helper
)