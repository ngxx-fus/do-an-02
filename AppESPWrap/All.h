#ifndef __APP_ESP_WRAP_ALL_H__
#define __APP_ESP_WRAP_ALL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PRINT_HEADER_COMPILE_MESSAGE
#pragma message ("AppESPWrap/All.h")
#endif

#include <stdint.h>
#include <stdlib.h>

#include "./ESPGPIOWrapper.h"
#include "./ESPLogWrapper.h"
#include "./ESPFreeRTOSWrapper.h"


#ifdef __cplusplus
}
#endif

#endif /// __APP_ESP_WRAP_ALL_H__