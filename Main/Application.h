#ifndef __APPLICATION_H__
#define __APPLICATION_H__


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

    void AppInitialize(){
        SysEntry("AppInitialize()");

        #if (SYSTEM_MON_EN == 1)
            SysLog("[AppInitialize] [+Task] TaskSystemMonitor");
            CreateTaskCPU0(TaskSystemMonitor, "TaskSystemMonitor", 4096, NULL, 2, NULL);
        #endif

        SysLog("[AppInitialize] [+Task] TaskScreen");
        CreateTaskCPU1(TaskScreen, "TaskScreen", 4096, NULL, 2, NULL);

        #if (ANALYZER_READER_COM_EN == 1)
            SysLog("[AppInitialize] [+Task] TaskAnalyzerReaderCom");
            CreateTaskCPU0(TaskAnalyzerReaderCom, "TaskAnalyzerReaderCom", 4096, NULL, 3, NULL);
        #endif

        SysExit("AppInitialize()");
    }


#endif /// (FIRMWARE_TYPE == TYPE_ANALYZER_MASTER)

#if (FIRMWARE_TYPE == TYPE_ANALYZER_READER)
    #include "../AppCore/AnalyzerReader/All.h"

    void AppInitialize(){
        SysEntry("AppInitialize()");

        #if (SYSTEM_MON_EN == 1)
            SysLog("[AppInitialize] [+Task] TaskSystemMonitor");
            CreateTaskCPU0(TaskSystemMonitor, "TaskSystemMonitor", 4096, NULL, 2, NULL);
        #endif

        SysLog("[AppInitialize] [+Task] TaskMonitor");
        CreateTaskCPU0(TaskMonitor, "TaskMonitor", 4096, NULL, 2, NULL);

        #if (ANALYZER_READER_COM_EN == 1)
            SysLog("[AppInitialize] [+Task] TaskAnalyzerReader");
            CreateTaskCPU1(TaskAnalyzerReader, "TaskAnalyzerReader", 4096, NULL, 3, NULL);
        #endif

        SysExit("AppInitialize()");
    }
#endif /// (FIRMWARE_TYPE == TYPE_ANALYZER_READER)

#ifdef __cplusplus
}
#endif

#endif