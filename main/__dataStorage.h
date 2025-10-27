#ifndef __dataStorage_H__
#define __dataStorage_H__

/// CIRCULAR LOG CONTROL //////////////////////////////////////////////////////////////////////////

#ifndef CIRCULAR_BUFFER_LOG_LEVEL
    #define CIRCULAR_BUFFER_LOG_LEVEL    1
#endif

#ifdef CIRCULAR_BUFFER_LOG_LEVEL

    #if (CIRCULAR_BUFFER_LOG_LEVEL >= 1)
        #define __cBuffErr(...)   __sys_err(__VA_ARGS__)
    #else
        #define __cBuffErr(...)
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
    CIRCULAR_BUFFER_NOT_FULL = 0,   /// Buffer is not full
    CIRCULAR_BUFFER_FULL     = 1,   /// Buffer is full
    CIRCULAR_BUFFER_STATUS_NUMBER,
};

typedef struct circularBuffer_t {
    cBuffData_t *   buffer;             /// Pointer to the buffer memory
    size_t          front;              /// Index of the front 
    size_t          back;               /// Index of the back 
    size_t          maxSize;            /// Maximum size of the buffer
    flag_t          status;             /// Flag indicating if the buffer is full
    portMUX_TYPE    mutex;              /// Mutex for thread-safe access
} circularBuffer_t;

/// FUNCTION PROTOTYPES ////////////////////////////////////////////////////////////////////////////

/// @brief Create a new circular buffer object
/// @param cBuffPtr Pointer to the circular buffer object pointer
/// @param cBuffSize Size of the circular buffer
/// @return OKE if successful, error code otherwise
def circularBufferCreate(circularBuffer_t ** cBuffPtr, size_t cBuffSize)

/// @brief Push new data to the back of the circular buffer
/// @param cBuffPtr Pointer to the circular buffer object pointer
/// @param newData New data to be added
/// @return OKE if successful, error code otherwise
def cBuffPushBack(circularBuffer_t ** cBuffPtr, cBuffData_t newData);

/// @brief Pop data from the back of the circular buffer
/// @param cBuffPtr Pointer to the circular buffer object pointer
/// @param dataOut Pointer to store the popped data
/// @return OKE if successful, error code otherwise
def cBuffPopFront(circularBuffer_t ** cBuffPtr, cBuffData_t * dataOut);

/// @brief Push new data to the front of the circular buffer
/// @param cBuffPtr Pointer to the circular buffer object pointer
/// @param newData  New data to be added
/// @return  OKE if successful, error code otherwise
def cBuffPushFront(circularBuffer_t ** cBuffPtr, cBuffData_t newData);

/// @brief Pop data from the front of the circular buffer
/// @param cBuffPtr Pointer to the circular buffer object pointer
/// @param dataOut Pointer to store the popped data
/// @return OKE if successful, error code otherwise
def cBuffPopBack(circularBuffer_t ** cBuffPtr, cBuffData_t * dataOut);

/// FUNCTION DEFINITIONS //////////////////////////////////////////////////////////////////////////


def cBuffCreate(circularBuffer_t ** cBuffPtr, size_t cBuffSize){
    __cBuffEntry("circularBufferCreate(%p, %d)", cBuffPtr, cBuffSize);

    /// Safety check for input parameters
    if(__is_null(cBuffPtr) || __isnot_positive(cBuffSize)){
        goto returnErrInvalidParam;
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
    DATA(cBuffPtr)->front = 0;
    DATA(cBuffPtr)->back = 0;
    DATA(cBuffPtr)->status = 0;
    portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;
    dev->mutex = mutex;

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

def cBuffPushBack(circularBuffer_t ** cBuffPtr, cBuffData_t newData){
    __cBuffEntry("cBuffPushBack(%p, %d)", cBuffPtr, newData);

    __cBuffExit("cBuffPushBack() : OKE");
    return OKE;
}

def cBuffPushFront(circularBuffer_t ** cBuffPtr, cBuffData_t newData){
    __cBuffEntry("cBuffPushFront(%p, %d)", cBuffPtr, newData);
    
    __cBuffExit("cBuffPushFront() : OKE");
    return OKE;
}

def cBuffPopBack(circularBuffer_t ** cBuffPtr, cBuffData_t * dataOut){
    __cBuffEntry("cBuffPopBack(%p, %p)", cBuffPtr, dataOut);

    __cBuffExit("cBuffPopBack() : OKE");
    return OKE;
}

def cBuffPopFront(circularBuffer_t ** cBuffPtr, cBuffData_t * dataOut){
    __cBuffEntry("cBuffPopFront(%p, %p)", cBuffPtr, dataOut);

    __cBuffExit("cBuffPopFront() : OKE");
    return OKE;
}





#endif 