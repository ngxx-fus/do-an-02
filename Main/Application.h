#ifndef __APPLICATION_H__
#define __APPLICATION_H__


#include "AnalyzerMaster/AnalyzerMaster.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef PRINT_HEADER_COMPILE_MESSAGE
#pragma message ("Main/Application.h")
#endif

#include "../AppConfig/All.h"
#include "../AppUtils/All.h"
#include "../AppESPWrap/All.h"

#include "../AppCore/SystemMonitor/All.h"

#if (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)
    #include "../AppCore/AnalyzerMaster/All.h"
#endif /// (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)

#if (FIRMWARE_TYPE == TYPE_ANALYZER_READER)
    #include "../AppCore/AnalyzerReader/All.h"
#endif /// (FIRMWARE_TYPE == TYPE_ANALYZER_READER)

void AppInitialize(){
    SysEntry("AppInitialize()");
   
    #if (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)
        AnalyzerMasterInit();    
    #endif /// (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)

    #if (FIRMWARE_TYPE == TYPE_ANALYZER_READER)
        AnalyzerReaderInit();    
    #endif /// (FIRMWARE_TYPE == TYPE_ANALYZER_READER)

    SysExit("AppInitialize()");
}

#ifdef __cplusplus
}
#endif

#endif
