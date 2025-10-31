#ifndef __C_BUFF_H__
#define __C_BUFF_H__

#include "../../helper/general.h"

/// CIRCULAR LOG CONTROL //////////////////////////////////////////////////////////////////////////

#ifndef CIRCULAR_BUFFER_LOG_LEVEL
    #define CIRCULAR_BUFFER_LOG_LEVEL    1
#endif

#ifdef CIRCULAR_BUFFER_LOG_LEVEL

    #if (CIRCULAR_BUFFER_LOG_LEVEL >= 1)
        #define __cBuffErr(...)   G_LEVEL >=__sys_err(__VA_ARGS__)
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
def cBuffCreate(circularBuffer_t ** cBuffPtr, size_t cBuffSize);

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


#endif