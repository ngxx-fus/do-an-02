# CMakeLists.txt for SystemMonitor component
idf_component_register(
    SRCS        "SystemMonitor.c"
    INCLUDE_DIRS "."
    REQUIRES    AppESPWrap AppUtils AppConfig
)