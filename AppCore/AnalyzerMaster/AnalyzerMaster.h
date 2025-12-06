#ifndef __ANALYZER_MASTER_H__
#define __ANALYZER_MASTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PRINT_HEADER_COMPILE_MESSAGE
#pragma message ("AppCore/AnalyzerMaster/AnalyzerMaster.h")
#endif

#include "../../AppConfig/All.h"
#include "../../AppUtils/All.h"
#include "../../AppESPWrap/All.h"
#include "../../AppFonts/All.h"


#include "../../AppComponents/LCD32/LCD32.h"

extern LCD32Dev_t * lcd32; 

void TaskScreen(void * pv);

#ifdef __cplusplus
}
#endif

#endif /// __ANALYZER_MASTER_H__
