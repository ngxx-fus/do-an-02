#ifndef __CONFIG_COMPONENTS_H__
#define __CONFIG_COMPONENTS_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PRINT_HEADER_COMPILE_MESSAGE
#pragma message ("AppConfig/Components.h")
#endif

#include <stdint.h>
#include <stdlib.h>

#define ENABLE                  0
#define DISABLE                 1

#define EN_DRIVER_P16           ENABLE
#define EN_DRIVERLCD            ENABLE

#ifdef __cplusplus
}
#endif


#endif /// __CONFIG_COMPONENTS_H__