#ifndef __dataStorage_H__
#define __dataStorage_H__

#include "../helper/general.h"

/// CIRCULAR LOG CONTROL //////////////////////////////////////////////////////////////////////////

#ifndef CIRCULAR_BUFFER_LOG_LEVEL
    #define CIRCULAR_BUFFER_LOG_LEVEL    1
#endif

#ifdef CIRCULAR_BUFFER_LOG_LEVEL

    #if (CIRCULAR_BUFFER_LOG_LEVEL >= 1)
        #define __cBuffEr   r(...)   G_LEVEL >=__sys_err(__VA_ARGS__)
    #else
        #define push-back __cBuffErr(...)
    #endif

    #if (CIRCULAR_BUFFER_LOG_LEVEL >= 2)
        #define __cBuffEntry(...)   __sys_entry(__VA_ARGS__)
        #define __cBuffExit(...)    __sys_exit(__VA_ARGS__)
        #define __cBuffWarn(...)    __sys_warn(__VA_ARGS__)
    #else
        #define __cBuffEntry(...)
        #define __cBuffExit(...)
        #define __cBuffWarn(...)
    #endif

    #if (CIRCULAR_BUFFER_LOG_LEVEL >= 3)
        #define __cBuffLog(...)     __sys_log(__VA_ARGS__)
        #define __cBuffInfo(...)    __sys_info(__VA_ARGS__)
    #else
        #define __cBuffLog(...)
        #define __cBuffInfo(...)
    #endif

    #if (CIRCULAR_BUFFER_LOG_LEVEL >= 4)
        #define __cBuffLog1(...)    __sys_log1(__VA_ARGS__)
    #else
        #define __cBuffLog1(...)
    #endif

#endif

/// CIRCULAR BUFFER FOR DATA STORAGE //////////////////////////////////////////////////////////////

typedef uint16_t cBuffData_t;

enum CIRCULAR_BUFFER_STATUS {
    CBUFF_FULL    = 0,   /// Buffer is full
    CBUFF_STATUS_NUMBER,
};

typedef struct circularBuffer_t {
    cBuffData_t *   buffer;             /// Pointer to the buffer memory
    size_t          head;               /// Index of the head 
    size_t          tail;               /// Index of the back 
    size_t          curSize;            /// Current size of the buffer
    size_t          maxSize;            /// Maximum size of the buffer
    flag_t          status;             /// (legacy) Flag indicating if the buffer is full
    portMUX_TYPE    mutex;              /// Mutex for thread-safe access
} circularBuffer_t;

/*
NOTE:
    - Empty: 

*/

/// HELPER MACROS //////////////////////////////////////////////////////////////////////////////////

#define __cBuffNullCheck(obj, objName, funcName, ...)       \
    do {                                                    \
        if (__is_null(obj)) {                               \
            __cBuffErr("[%s] %s is NULL!", funcName, objName); \
            __VA_OPT__(__VA_ARGS__)                         \
        }                                                   \
    } while (0)

#define C_ADD(expr, boundary)        ((expr)%(boundary))
#define C_SUB(expr, boundary)        (((expr)+boundary)%(boundary))

/// FUNCTION PROTOTYPES ////////////////////////////////////////////////////////////////////////////

/// @brief Create a new circular buffer object
/// @param cBuffPtr Pointer to the circular buffer object pointer
/// @param cBuffSize Size of the circular buffer
/// @return OKE if successful, error code otherwise
def circularBufferCreate(circularBuffer_t ** cBuffPtr, size_t cBuffSize);

/// @brief Push new data to the back of the circular buffer
/// @param cBuff Pointer to the circular buffer object pointer
/// @param newData New data to be added
/// @return OKE if successful, error code otherwise
def cBuffPushBack(circularBuffer_t * cBuff, cBuffData_t newData);

/// @brief Pop data from the back of the circular buffer
/// @param cBuff Pointer to the circular buffer object pointer
/// @param dataOut Pointer to store the popped data
/// @return OKE if successful, error code otherwise
def cBuffPopFront(circularBuffer_t * cBuff, cBuffData_t * dataOut);

/// @brief Push new data to the front of the circular buffer
/// @param cBuff Pointer to the circular buffer object pointer
/// @param newData  New data to be added
/// @return  OKE if successful, error code otherwise
def cBuffPushFront(circularBuffer_t * cBuff, cBuffData_t newData);

/// @brief Pop data from the front of the circular buffer
/// @param cBuff Pointer to the circular buffer object pointer
/// @param dataOut Pointer to store the popped data
/// @return OKE if successful, error code otherwise
def cBuffPopBack(circularBuffer_t * cBuff, cBuffData_t * dataOut);

/// FUNCTION DEFINITIONS //////////////////////////////////////////////////////////////////////////

def cBuffCreate(circularBuffer_t ** cBuffPtr, size_t cBuffSize){
    __cBuffEntry("circularBufferCreate(%p, %d)", cBuffPtr, cBuffSize);

    /// Safety check for input parameters
    if(__is_null(cBuffPtr)){
        goto returnErrInvalidParam;
    }

    /// Set default size if invalid size is provided
    if(__isnot_positive(cBuffSize)){
        cBuffSize = 64;
    }

    /// Create new circular buffer object then check memory allocation
    DATA(cBuffPtr) = malloc(sizeof(circularBuffer_t));
    if (__is_null(DATA(cBuffPtr))){
        goto returnErrMallocFailed;
    }

    /// Assign size and allocate buffer memory
    #if (CBUFF_USE_PSRAM_FOR_CIRCULAR_BUFF == 1) 
        /// TODO: Complete PSRAM allocation
    #else 
        DATA(cBuffPtr)->buffer = malloc(sizeof(uint8_t) * cBuffSize);
    #endif

    if ( __is_null(DATA(cBuffPtr)->buffer) ){
        /// Free previously allocated circular buffer object
        free(DATA(cBuffPtr));
        /// Assign NULL to prevent dangling pointer
        DATA(cBuffPtr) = NULL;
        goto returnErrMallocFailed;
    }

    /// Initialize circular buffer parameters
    DATA(cBuffPtr)->maxSize = cBuffSize;
    DATA(cBuffPtr)->head = 0;
    DATA(cBuffPtr)->tail = 0;
    DATA(cBuffPtr)->status = 0;
    portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;
    DATA(cBuffPtr)->mutex = mutex;

    /// Exit function 
    __cBuffExit("circularBufferCreate() : OKE");
    return OKE;

    returnErrInvalidParam:
        __cBuffExit("circularBufferCreate() : ERR_INVALID_ARG");
        return ERR_INVALID_ARG;
    
    returnErrMallocFailed:
        __cBuffExit("circularBufferCreate() : ERR_MALLOC_FAILED");
        return ERR_MALLOC_FAILED;

}

def cBuffClear(circularBuffer_t * cBuff){
    __cBuffEntry("cBuffClear(%p)", cBuff);
    __cBuffNullCheck(cBuff, "cBuff", "cBuffPushBack", goto returnErrNull;);

    cBuff->curSize  = 0;
    cBuff->head     = 0;
    cBuff->tail     = 0;

    __cBuffExit("cBuffClear() : OKE");
    return OKE;
    returnErrNull:
        __cBuffExit("cBuffClear() : ERR_NULL");
        return ERR_NULL;
}

def cBuffPushBack(circularBuffer_t * cBuff, cBuffData_t newData){
    __cBuffEntry("cBuffPushBack(%p, %d)", cBuff, newData);
    __cBuffNullCheck(cBuff, "cBuff", "cBuffPushBack", goto returnErrNull;);

    /// Over-write
    if(cBuff->curSize >= cBuff->maxSize){
        /// Move head next (prevent bite itself)
        cBuff->head = C_ADD(1 + cBuff->head, cBuff->maxSize);
    }
    /// Normal push-back 
    else {
        /// Increase current size
        cBuff->curSize = (cBuff->curSize + 1);
    }
    /// Assign new data
    (cBuff->buffer)[cBuff->tail] = newData;
    /// Move tail next
    cBuff->tail = C_ADD(1 + cBuff->tail, cBuff->maxSize);

    __cBuffExit("cBuffPushBack() : OKE");
    return OKE;
    returnErrNull:
        __cBuffExit("cBuffPushBack() : ERR_NULL");
        return ERR_NULL;
}

def cBuffPushFront(circularBuffer_t * cBuff, cBuffData_t newData){
    __cBuffEntry("cBuffPushFront(%p, %d)", cBuff, newData);
    __cBuffNullCheck(cBuff, "cBuff", "cBuffPushFront", goto returnErrNull;);

    /// Over-write
    if(cBuff->curSize >= cBuff->maxSize){
        /// Move tail prev (prevent bite itself)
        cBuff->tail = C_SUB((cBuff->tail) - 1, cBuff->maxSize);
    }
    /// Normal push-back 
    else {
        /// Increase current size
        cBuff->curSize = (cBuff->curSize + 1);
    }
    /// Move head prev
    cBuff->head = C_SUB((cBuff->head) - 1, cBuff->maxSize);
    /// Assign new data
    (cBuff->buffer)[cBuff->head] = newData;

    __cBuffExit("cBuffPushFront() : OKE");
    return OKE;
    returnErrNull:
        __cBuffExit("cBuffPushFront() : ERR_NULL");
        return ERR_NULL;
}

def cBuffPopBack(circularBuffer_t * cBuff, cBuffData_t * dataOut){
    __cBuffEntry("cBuffPopBack(%p, %p)", cBuff, dataOut);
    __cBuffNullCheck(cBuff, "cBuff", "cBuffPopBack", goto returnErrNull;);

    /// Check if posible to pop the data (not empty)
    if(__isnot_positive(cBuff->curSize)){
        goto returnErr;
    }else{
        cBuff->curSize = cBuff->curSize - 1;
    }
    /// Move tail pre
    ((cBuff)->tail) = C_SUB((cBuff->tail)-1, cBuff->maxSize);
    /// Export data 
    (*dataOut) = (cBuff->buffer)[cBuff->tail];

    __cBuffExit("cBuffPopBack() : OKE");
    return OKE;
    returnErr:
        __cBuffExit("cBuffPopFront() : ERR");
        return ERR;
    returnErrNull:
        __cBuffExit("cBuffPopBack() : ERR_NULL");
        return ERR_NULL;
}

def cBuffPopFront(circularBuffer_t * cBuff, cBuffData_t * dataOut){
    __cBuffEntry("cBuffPopFront(%p, %p)", cBuff, dataOut);
    __cBuffNullCheck(cBuff, "cBuff", "cBuffPopFront", goto returnErrNull;);

    /// Check if posible to pop the data (not empty)
    if(__isnot_positive(cBuff->curSize)){
        goto returnErr;
    }else{
        cBuff->curSize = cBuff->curSize - 1;
    }
    /// Export data
    (*dataOut) = (cBuff->buffer)[((cBuff)->head)];
    /// Move head next    
    ((cBuff)->head) = C_ADD((cBuff->head)+1, cBuff->maxSize);

    __cBuffExit("cBuffPopFront() : OKE");
    return OKE;
    returnErr:
        __cBuffExit("cBuffPopFront() : ERR");
        return ERR;
    returnErrNull:
        __cBuffExit("cBuffPopFront() : ERR_NULL");
        return ERR_NULL;
}

/// DATA STOREGE //////////////////////////////////////////////////////////////////////////////////


def dataStorageInitilize(){


    return OKE;
}

void dataControlTask(void* pv){
    WAIT_SYSTEM_INIT_COMPLETED();
    __entry("dataControlTask()");

    vTaskDelay(0);
    __exit("dataControlTask()");
}

#endif 