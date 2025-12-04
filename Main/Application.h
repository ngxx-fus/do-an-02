#ifndef __APPLICATION_H__
#define __APPLICATION_H__


#include "../__config/all.h"

#if ESP32_DEVICE_MODE == SENDER
    #include "sender.h"
#endif
#if ESP32_DEVICE_MODE == RECEIVER
    #include "receiver.h"
#endif
#if ESP32_DEVICE_MODE == MONITOR
    #include "monitor.h"
#endif






#endif