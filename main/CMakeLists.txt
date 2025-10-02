idf_component_register(
    SRCS 
        "do-an-02.c"
        "../driver/spi/*.c"
        "../driver/i2c*.c"
        
    INCLUDE_DIRS 
        "."
        "../helper"
        "../driver/spi"
        "../driver/i2c"
)