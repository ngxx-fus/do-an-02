#ifndef __GENERAL_CONFIG_H__
#define __GENERAL_CONFIG_H__

/*
    This file contains only macros used as constants.
    To add, edit, or remove any enums, structs, or si-
    milar definitions, refer to the corresponding con-
    figuration headers (e.g., i2cDefinitions.h, oled1-
    28x64Definitions.h, etc.). For general configurat-
    ions such as return types or default enums, please 
    check the headers inside the helper/ folder.These 
    headers wrap all existing definitions from ESP-IDF
    /RTOS.

    To add a global variable, please add it into gene-
    ral header file in helper with volatile and extern
    type. After that, please declare a real variable
    in .c file (like main.c). 

    Directory tree:
    .
    ├── CMakeLists.txt
    ├── comDriver
    │   ├── i2c
    │   │   ├── CMakeLists.txt
    │   │   ├── i2c.c
    │   │   ├── i2c.h
    │   │   ├── i2cDefinition.h
    │   │   ├── i2cHelper.h
    │   │   └── i2cLog.h
    │   ├── oled128x64
    │   │   ├── CMakeLists.txt
    │   │   ├── oled128x64.c
    │   │   ├── oled128x64.h
    │   │   ├── oled128x64Commands.h
    │   │   ├── oled128x64Definitions.h
    │   │   ├── oled128x64Helpers.h
    │   │   └── oled128x64Log.h
    │   └── spi
    │       ├── CMakeLists.txt
    │       ├── spi.c
    │       └── spi.h
    ├── config
    │   ├── CMakeLists.txt
    │   ├── generalConfig.h
    │   └── pinConfig.h
    ├── diagram
    │   ├── diagram01.drawio
    │   └── diagram02.drawio
    ├── fonts
    │   ├── CMakeLists.txt
    │   ├── fontUtils.c
    │   ├── fontUtils.h
    │   └── gfxfont.h
    ├── helper
    │   ├── CMakeLists.txt
    │   ├── espGPIOWrapper.h
    │   ├── espLogWrapper.h
    │   ├── espRTOSWrapper.h
    │   ├── flag.h
    │   ├── general.h
    │   ├── helper.c
    │   ├── helper.h
    │   ├── param.h
    │   └── return.h
    ├── main
    │   ├── CMakeLists.txt
    │   ├── do-an-02.c
    │   ├── receiver.h
    │   └── sender.h
    ├── readme.md
    ├── sdkconfig
    └── sdkconfig.old

*/

#define SENDER              0x0
#define RECEIVER            0x1

#define ESP32_DEVICE_MODE   RECEIVER

/// To config font, check fontUtils.c in fonts/
/// Currently, the font is: TomThumb

/// To config the com protocol at begin,
/// See ENUM_SYSTEM_MODE enum in helper/general.h.
#define STARTUP_COM_MODE 0

/// Slave address for I2C comunication
#define SLAVE_ADDRE      0x39

/// Time interval of generating data frame
/// Unit: tick 
#define DATAFR_INTERVAL  200

/// Debouncing time for button
/// Unit: micro-sec
#define DEBOUNCE_TIME   10000

/// Short press thresold
/// Unit: micro-sec
#define SHORTPRESS_TIME 500000

/// Long press thresold
/// Unit: micro-sec
#define LONGPRESS_TIME 2000000

#define COM_SPI_FREQ   100000
#define COM_SPI_CONF   SPI_11_SLAVE

#endif
