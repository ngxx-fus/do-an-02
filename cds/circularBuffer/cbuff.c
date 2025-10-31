#include "cbuff.h"

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