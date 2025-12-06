# About project

This is one of three projects (Project 01, Project 02, and the Graduation Project - Capstone Project). In this project, I will develop a device that analyzes data on SPI/I2C buses and captures analog signals for display like an oscilloscope.

# About team

My team consists of two members:
- Nguyen Thanh Phu – Student ID: 22119211 – Contact: nthanhphu.k22.hcmute@gmail.com (myself)
- Nguyen Van Quoc – Student ID: 22119222 – Contact: quocvan561@gmail.com

# About device

## General system architecture

The image below describe the blocks diagram of the device.

<img src="imgs/SysArchBlockDiagram.png" style="width:500px">

# Project Structure

The project follows a layered and modular architecture designed for clarity, scalability, and ease of maintenance. Each layer has a distinct responsibility, promoting separation of concerns.

## Core Principles

-   **Modularity**: The system is divided into independent components (e.g., `LCD32`, `P16Com`). Each component is self-contained with its own `CMakeLists.txt`.
-   **Layered Architecture**:
    -   **Hardware Abstraction Layer (HAL)**: `AppESPWrap` provides a clean interface over the ESP-IDF, isolating the rest of the application from framework-specific details.
    -   **Driver/Component Layer**: `AppComponents` contains reusable drivers for hardware peripherals like the LCD.
    -   **Application Core Layer**: `AppCore` implements the main business logic of the device.
    -   **Entry Point**: `AppEntry` (named `Main` in this project) contains the `app_main` function, which initializes and starts the system.
-   **Centralized Includes**: Each module (e.g., `AppUtils`, `AppConfig`) has an `All.h` file. Including this single file provides access to all public interfaces of that module, simplifying dependency management.

## Top-Level Directory Structure

```
.
├── AppComponents/      -> Reusable hardware driver components.
├── AppCore/            -> High-level application logic.
├── AppConfig/          -> Project-wide configurations (pins, features).
├── Main/               -> Main application entry point (app_main).
├── AppFonts/           -> Font data and utilities.
├── AppUtils/           -> General-purpose utilities and helpers.
├── AppESPWrap/         -> Wrappers for ESP-IDF functions.
├── CMakeLists.txt      -> Main CMake build configuration.
├── diagrams/           -> System architecture and design diagrams.
├── readme.md           -> This file.
└── sdkconfig           -> ESP-IDF project configuration.
```

---

## Detailed Module Descriptions

### `Main` (`AppEntry`)
This is the starting point of the application.
-   `main.c`: Contains the `app_main` function. Its primary role is to initialize system services, configure hardware, and create the main application tasks defined in `AppCore`.

### `AppCore`
This layer contains the core logic and state machines of the device.
-   `AnalyzerMaster/`: Implements the main functionality of the logic analyzer, such as handling user input, managing the screen task (`TaskScreen`), and coordinating data acquisition.

### `AppComponents`
This directory houses all the low-level hardware drivers. Each driver is a self-contained component.
-   `P16Com/`: A generic driver for 16-bit parallel communication, which serves as a base for other drivers.
-   `LCD32/`: The specific driver for the ILI9341 3.2" LCD, built on top of `P16Com`. It handles initialization, drawing primitives, and canvas management.

### `AppConfig`
This is the central hub for all project configurations.
-   `pinConfig.h`: Defines all GPIO pin assignments for the hardware.
-   `projectConfig.h`: Contains feature flags, buffer sizes, and other compile-time settings.
-   `All.h`: Includes all other configuration files for easy access from other modules.

### `AppESPWrap`
This layer abstracts the underlying ESP-IDF framework. By wrapping ESP-IDF functions, we can easily swap out the framework or port the code to another platform in the future.
-   `espGPIOWrapper.h`: Wraps `driver/gpio.h`.
-   `espLogWrapper.h`: Wraps `esp_log.h`.
-   `espRTOSWrapper.h`: Wraps FreeRTOS headers like `freertos/FreeRTOS.h`, `freertos/task.h`, etc.

### `AppUtils`
A collection of general-purpose helper functions and macros used throughout the project.
-   `helper.h`/`.c`: Contains utilities for things like status codes, common macros (`IsNull`, `IsNotNull`), and other miscellaneous functions.

### `AppFonts`
Contains font data (e.g., GFX fonts) and utilities for rendering text.

---

## How to Add a New Component

Adding a new component (e.g., a new sensor driver) is straightforward.

**1. Create the Component Directory:**
Create a new directory inside `AppComponents/`, for example, `AppComponents/BME280/`.

**2. Add Source Files:**
Inside `AppComponents/BME280/`, add your source files (`BME280.c`, `BME280.h`).

**3. Create `CMakeLists.txt`:**
Create a `CMakeLists.txt` file inside `AppComponents/BME280/` with the following template. List all your source files and any other components it depends on.

```cmake
# CMakeLists.txt for BME280 component
idf_component_register(
    SRCS        "BME280.c"
    INCLUDE_DIRS "."
    REQUIRES    AppESPWrap AppUtils AppConfig # Add dependencies here
)
```
> **Note:** Common dependencies like `AppESPWrap`, `AppUtils`, and `AppConfig` are almost always required.

**4. Register the Component in the Main Build:**
Open the main `CMakeLists.txt` at the project root and add the path to your new component to `EXTRA_COMPONENT_DIRS`.

```cmake
# Root CMakeLists.txt
set(EXTRA_COMPONENT_DIRS
    ...
    "AppComponents/LCD32"
    "AppComponents/P16Com"
    "AppComponents/BME280"  # <-- Add your new component here
)
```

**5. Include and Use:**
Finally, include the component's header in your application code (e.g., in `AppCore/AnalyzerMaster/AnalyzerMaster.c`) and use it.

```c
#include "BME280.h" // From your new component

void SomeTask(void *pv){
    // ...
    BME280_Init();
    // ...
}
```

This modular approach ensures that the project remains clean and easy to manage as it grows.

---

# Useful links

- ESP32-S3 N16R8 DevKitC Module 

- 3.2inch 320x240 Touch LCD (D)

---

# Full Project Tree and File Descriptions

Below is the complete file tree of the project, along with a description of each key file and directory.

```
.
├── AppComponents
│   ├── LCD32
│   │   ├── CMakeLists.txt
│   │   ├── LCD32.c
│   │   ├── LCD32.h
│   │   ├── LCD32Cmds.h
│   │   └── LCD32Colors.h
│   └── P16Com
│       ├── CMakeLists.txt
│       ├── P16Com.c
│       └── P16Com.h
├── AppConfig
│   ├── All.h
│   ├── CMakeLists.txt
│   ├── Components.h
│   ├── DevicePinout.h
│   ├── FirmwareType.h
│   └── SystemLog.h
├── AppCore
│   ├── AnalyzerMaster
│   │   ├── All.h
│   │   ├── AnalyzerMaster.c
│   │   ├── AnalyzerMaster.h
│   │   └── CMakeLists.txt
│   └── AnalyzerReader
│       ├── AnalyzerReader.c
│       ├── AnalyzerReader.h
│       └── CMakeLists.txt
├── AppESPWrap
│   ├── All.h
│   ├── AppESPWrap.c
│   ├── CMakeLists.txt
│   ├── ESPFreeRTOSWrapper.h
│   ├── ESPGPIOWrapper.h
│   └── ESPLogWrapper.h
├── AppFonts
│   ├── All.h
│   ├── AppFont.c
│   ├── CMakeLists.txt
│   ├── gfxfont.h
│   └── localFonts/
├── AppUtils
│   ├── All.h
│   ├── AppUtils.c
│   ├── Arithmetic.h
│   ├── BitOp.h
│   ├── CMakeLists.txt
│   ├── FlagControl.h
│   ├── Loop.h
│   └── ReturnType.h
├── CMakeLists.txt
├── Doxyfile
├── Main
│   ├── Application.c
│   ├── Application.h
│   └── CMakeLists.txt
├── build/
├── docs/
├── imgs/
├── readme.md
├── sdkconfig
└── tree
```

### File Descriptions

-   **`AppComponents/`**: Contains all reusable hardware driver components.
    -   **`LCD32/`**: Driver for the 3.2" ILI9341 LCD.
        -   `LCD32.h`/`.c`: Public interface and implementation for the LCD driver.
        -   `LCD32Cmds.h`: Defines all command codes for the ILI9341 controller.
        -   `LCD32Colors.h`: Defines a palette of pre-set colors.
    -   **`P16Com/`**: A generic, low-level driver for 16-bit parallel communication.
        -   `P16Com.h`/`.c`: Interface and implementation for sending/receiving data over a 16-bit parallel bus. It forms the base for the `LCD32` driver.
-   **`AppConfig/`**: Central hub for all project-wide configurations.
    -   `All.h`: A master include file for the configuration module.
    -   `DevicePinout.h`: Defines all physical GPIO pin assignments for the hardware.
    -   `FirmwareType.h`: Defines the type of firmware being built (e.g., Master or Reader).
    -   `SystemLog.h`: Configures logging levels and settings for different modules.
-   **`AppCore/`**: Implements the high-level application logic and state machines.
    -   **`AnalyzerMaster/`**: Contains the core logic for the "Master" device firmware.
        -   `AnalyzerMaster.c`: Implements the main application task (`TaskScreen`) and business logic.
    -   **`AnalyzerReader/`**: Contains the core logic for the "Reader" device firmware.
-   **`AppESPWrap/`**: Hardware Abstraction Layer (HAL) that wraps ESP-IDF functions.
    -   `ESPFreeRTOSWrapper.h`: Provides convenient macros for FreeRTOS features (tasks, mutexes, delays).
    -   `ESPGPIOWrapper.h`: Wraps ESP-IDF GPIO functions and provides fast, direct register access macros for high-performance I/O.
    -   `ESPLogWrapper.h`: Wraps ESP-IDF logging functions to standardize log output.
-   **`AppFonts/`**: Manages font data and rendering utilities.
    -   `gfxfont.h`: Defines the core data structures for GFX-style fonts.
    -   `localFonts/`: A directory containing all the individual font header files.
-   **`AppUtils/`**: A collection of general-purpose, project-wide helper functions and macros.
    -   `ReturnType.h`: Defines standard return types and status codes (e.g., `STAT_OKE`, `STAT_ERR_NULL`).
    -   `BitOp.h`, `FlagControl.h`, `Loop.h`: Provide useful macros for common low-level operations.
-   **`Main/`**: The main application entry point.
    -   `Application.c`: Contains the `app_main` function, which initializes the system and starts the main application tasks.
-   **`CMakeLists.txt` (Root)**: The main build script for the entire project, defining all components.
-   **`Doxyfile`**: Configuration file for the Doxygen documentation generator.
-   **`build/`**: Auto-generated directory containing all build artifacts (binaries, object files, etc.).
-   **`docs/`**: Auto-generated directory containing the Doxygen HTML documentation.
-   **`imgs/`**: Contains images and diagrams used in the documentation.
-   **`readme.md`**: This file.
-   **`sdkconfig`**: The main configuration file for the ESP-IDF project, generated by `menuconfig`.
-   **`tree`**: A text file containing the project's directory structure.