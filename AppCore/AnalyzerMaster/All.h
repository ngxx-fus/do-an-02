#ifndef __ANALYZER_MASTER_ALL_H__
#define __ANALYZER_MASTER_ALL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PRINT_HEADER_COMPILE_MESSAGE
#pragma message ("AppCore/AnalyzerMaster/All.h")
#endif

#include "AnalyzerMaster.h"

static inline void AnalyzerMasterInit(){
    SysEntry("AnalyzerMasterInit()");

    #if (EN_TASK_SYSTEM_MONITOR == ENABLE)
        SysLog("[AppInitialize] [+Task] TaskSystemMonitor");
        CreateTaskCPU0(TaskSystemMonitor, "TaskSystemMonitor", 4096, NULL, 2, NULL);
    #endif

    #if (EN_TASK_SCREEN_CONTROL == ENABLE)
        SysLog("[AppInitialize] [+Task] TaskScreen");
        CreateTaskCPU0(TaskScreen, "TaskScreen", 4096, NULL, 2, NULL);
    #endif /// EN_TASK_SCREEN_CONTROL

    #if (ANALYZER_READER_COM_EN == 1)
        SysLog("[AppInitialize] [+Task] TaskAnalyzerReaderCom");
        CreateTaskCPU1(TaskAnalyzerReaderCom, "TaskAnalyzerReaderCom", 4096, NULL, 3, NULL);
    #endif

    SysExit("AnalyzerMasterInit()");    
}

#ifdef __cplusplus
}
#endif

#endif /// __ANALYZER_MASTER_ALL_H__
