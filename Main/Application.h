#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#ifdef PRINT_HEADER_COMPILE_MESSAGE
#pragma message ("Main/Application.h")
#endif

/// @brief Include project config
#include "../AppConfig/All.h"

/// @brief Include project utilities
#include "../AppUtils/All.h"

/// @brief Include  ESP's functions and freeRTOS wrapper
#include "../AppESPWrap/All.h"

/// @brief Application's firmware
#if (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)
    #include "../AppCore/AnalyzerMaster/All.h"
#endif /// (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)


#endif