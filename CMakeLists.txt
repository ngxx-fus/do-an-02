cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(
    EXTRA_COMPONENT_DIRS        
    "Main"
    "AppCore"
    # "AppComponents"
    "AppESPWrap"
    "AppFonts"
    "AppUtils"
    "AppConfig"
)

project(AppMain)