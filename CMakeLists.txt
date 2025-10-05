# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Tell CMake to look for additional components in the 'comDriver',  directory
set(EXTRA_COMPONENT_DIRS
    # "helper"
    "comDriver/i2c"
    "comDriver/oled128x64"
    "comDriver/spi"
)

# Define the project name
project(do-an-02)