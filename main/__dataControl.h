#ifndef __DATACONTROL_H__
#define __DATACONTROL_H__

#include "../../helper/general.h"

#if (CIRCULAR_BUFFER_EN == 1)
    #include "../circularBuffer.h"
#endif  
#if (LINEAR_BUFFER_EN == 1) 
    /// TODO: Complete linear buffer for future
    #error("[Fus] Linear buffer is not defined!")
#endif

/// @brief Circular buffer for store data received from input-module
circularBuffer_t cBuff;

/// @brief Global data flag for controlling data storage operations
volatile flag_t dataStorageFlag = NONE_FLAG_SET;

/// @brief Mutex for synchronizing access to the data store flag
portMUX_TYPE dataStorageMutex = portMUX_INITIALIZER_UNLOCKED;

/// @brief Initial data control part
def dataStorageInitilize(){
    __sys_entry("dataStorageInitilize()");
    
    /// Initialize new buffer that store data from other side of system.
    cBuffCreate(&cBuff, CBUFF_MAX_SIZE);
    
    __sys_exit("dataStorageInitilize()");
    return OKE;
}

/// @brief Data control part
void dataControlTask(void* pv){
    WAIT_SYSTEM_INIT_COMPLETED();
    __entry("dataControlTask()");

    flag_t localdataStorageFlag;
    while(!IS_SYSTEM_STOPPED){
        /// Get and clear screenFlag atomically (get one time only)
        PERFORM_IN_CRITICAL(&dataStoreMutex,
            localdataStorageFlag = dataStorageFlag;
            screenFlag = 0;
        );

        /// Process screen operations based on the retrieved flags
        /// until all flags are handled. 
        while (__isnot_zero(localScreenFlag)){
            /// TODO: add the flag handle

            if(__isnot_zero(localScreenFlag)){
                __sys_err("[screenControlTask] Warning: Unrecognized screenFlag bits: 0x%08x", localScreenFlag);
                localScreenFlag = 0;
            }
        }

        vTaskDelay(0);
    }

    vTaskDelay(0);
    __exit("dataControlTask()");
}


#endif 