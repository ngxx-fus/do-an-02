#ifndef __ANALYZER_READER_ALL_H__
#define __ANALYZER_READER_ALL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PRINT_HEADER_COMPILE_MESSAGE
#pragma message ("AppCore/AnalyzerReader/All.h")
#endif

#include "AnalyzerReader.h"

static inline void AnalyzerReaderInit(){
    SysEntry("AnalyzerReaderInit()");

    #if (SYSTEM_MON_EN == 1)
        SysLog("[AppInitialize] [+Task] TaskSystemMonitor");
        CreateTaskCPU0(TaskSystemMonitor, "TaskSystemMonitor", 4096, NULL, 2, NULL);
    #endif

    #if (ANALYZER_READER_COM_EN == 1)
        SysLog("[AppInitialize] [+Task] TaskScreenTouchSPICom");
        CreateTaskCPU1(TaskScreenTouchSPICom, "TaskScreenTouchSPICom", 4096, NULL, 3, NULL);
    #endif

    #if (ANALYZER_READER_COM_EN == 1)
        SysLog("[AppInitialize] [+Task] TaskAnalyzerMasterCom");
        CreateTaskCPU1(TaskAnalyzerMasterCom, "TaskAnalyzerMasterCom", 4096, NULL, 3, NULL);
    #endif

    SysExit("AnalyzerReaderInit()"); 
}

#ifdef __cplusplus
}
#endif

#endif /// __ANALYZER_READER_ALL_H__