/**
 * @file safeCom.c
 * @brief Safe Communication Library Implementation
 */

#include "safeCom.h"

#if (SAFE_COM_EN == 1)

/* ========================================================================== */
/* VARIABLES                                                                  */
/* ========================================================================== */

scByte_t __scBuffer[SC_BUFFER_SIZE];
scByte_t __scIDKnownList[SC_KNOWN_LIST_SIZE];
scWord_t __scIDKnownPosList[SC_KNOWN_LIST_SIZE];

/* Buffer Management */
static volatile scSize_t __scHead = 0;      /// Write Index (ISR modifies)
static volatile scSize_t __scProcTail = 0;  /// Processing Index (Pooling task modifies)

/* Frame List Management (Ring Buffer) */
static volatile scSize_t __scFrameListHead = 0; /// Write Frame Index
static volatile scSize_t __scFrameListTail = 0; /// Read Frame Index
static volatile scSize_t __scFrameCount = 0;

/* Parsing State Machine */
static int __scParsingStartIdx = -1; /// -1: No header found yet

/* ========================================================================== */
/* INTERNAL HELPERS                                                           */
/* ========================================================================== */

static inline scSize_t __scNextIdx(scSize_t idx) {
    return (idx + 1) % SC_BUFFER_SIZE;
}

static inline scByte_t __scPeekAt(scSize_t idx) {
    return __scBuffer[idx % SC_BUFFER_SIZE];
}

/// @brief Remove oldest frame metadata (used when buffer overwrites or user consumes)
static void __scDiscardOldestFrameMeta(void) {
    if (__scFrameCount > 0) {
        __scFrameListTail = (__scFrameListTail + 1) % SC_KNOWN_LIST_SIZE;
        __scFrameCount--;
        __sc_log("SC: Auto discarded oldest frame due to full/overwrite");
    }
}

/// @brief Extract, Unstuff and Copy frame to user buffer
static scReturnVal_t __scExtractFrame(scSize_t startIdx, scByte_t* oHeader, scByte_t* oTrailer, scByte_t* oArr, scSize_t oArrSize) {
    scSize_t curr = __scNextIdx(startIdx);
    scSize_t payloadIdx = 0;
    
    /* 1. Get Header */
    if (oHeader) *oHeader = SC_HEADER_BYTE;

    /* 2. Traverse buffer to find Trailer and Unstuff */
    /* We assume the frame is valid since it's in the Known List */
    while (1) {
        scByte_t b = __scPeekAt(curr);

        if (b == SC_TRAILER_BYTE) {
            if (oTrailer) *oTrailer = b;
            break; 
        }

        /* Handle Stuffing */
        if (b == SC_ESC_BYTE) {
            curr = __scNextIdx(curr);
            b = __scPeekAt(curr) ^ SC_XOR_MASK;
        }

        /* Copy Data */
        /* Note: This includes the CRC bytes at the end of the payload for now */
        if (oArr && payloadIdx < oArrSize) {
            oArr[payloadIdx++] = b;
        } else if (payloadIdx >= oArrSize && oArr) {
             return SC_ERR_FULL; /* User buffer too small */
        }
        
        curr = __scNextIdx(curr);
        
        /* Safety break to prevent infinite loop if trailer missing (shouldn't happen for Known Frames) */
        if (curr == __scHead) break; 
    }

    /* 3. Strip CRC (Last 2 bytes) */
    if (payloadIdx >= 2) {
        if (oArr) {
            oArr[payloadIdx-1] = 0;
            oArr[payloadIdx-2] = 0;
        }
    } else {
        return SC_ERR_CRC;
    }

    return SC_OKE;
}

/// @brief Wait for a frame with timeout logic
static scReturnVal_t __scWaitForFrame(int32_t timeout) {
    if (__scFrameCount > 0) return SC_OKE;
    
    if (timeout == 0) return SC_ERR_TIMEOUT;
    
    int32_t wait = (timeout < 0) ? SC_DEF_WAIT_TIME : timeout;
    
    while (wait > 0) {
        #if (SC_MULTI_TASK_EN == 0)
        scPooling(); /* If not multi-task, we must pool here */
        #endif
        
        if (__scFrameCount > 0) return SC_OKE;
        
        scTaskDelay(1);
        wait--;
    }
    return SC_ERR_TIMEOUT;
}

/* ========================================================================== */
/* PUBLIC API                                                                 */
/* ========================================================================== */

scWord_t __scCRC16Cumpute(scWord_t* iPrev, scWord_t iWord, scWord_t* oWord) {
    scWord_t crc = (iPrev) ? *iPrev : 0xFFFF;
    scWord_t data = iWord;
#if (SC_HW_CRC_EN == 1)
    /* HW CRC Implementation Here */
#else
    /* CCITT-False (0x1021) or 0x2243 SW Implementation */
    crc ^= (data << 8);
    for (int i = 0; i < 8; i++) {
        if (crc & 0x8000)
            crc = (crc << 1) ^ SC_CRC_POLYNOMIAL;
        else
            crc = (crc << 1);
    }
#endif
    if (oWord) *oWord = crc;
    return crc;
}

scReturnVal_t scEncode(void* iArr, scSize_t iSize, void* oArr, scSize_t oArrSize) {
    __sc_entry();
    if (!iArr || !oArr) return SC_ERR_NULL;
    if (iSize <= 0) return SC_ERR_ARG;
    
    /* Worst case size check: Header + Trailer + (Size+CRC)*2 (all escaped) */
    if (oArrSize < (scSize_t)(2 + (iSize + 2) * 2)) return SC_ERR_FULL;

    scByte_t* pIn = (scByte_t*)iArr;
    scByte_t* pOut = (scByte_t*)oArr;
    scSize_t outIdx = 0;
    scWord_t crc = 0xFFFF;

    /* 1. Header */
    pOut[outIdx++] = SC_HEADER_BYTE;

    /* Stuffing Helper Macro */
    #define PUSH_STUFFED(byte) do { \
        if ((byte) == SC_HEADER_BYTE || (byte) == SC_TRAILER_BYTE || (byte) == SC_ESC_BYTE) { \
            pOut[outIdx++] = SC_ESC_BYTE; \
            pOut[outIdx++] = (byte) ^ SC_XOR_MASK; \
        } else { \
            pOut[outIdx++] = (byte); \
        } \
    } while(0)

    /* 2. Data */
    for (int i = 0; i < iSize; i++) {
        __scCRC16Cumpute(&crc, pIn[i], &crc);
        PUSH_STUFFED(pIn[i]);
    }

    /* 3. CRC (Big Endian usually preferred, but let's stick to Byte Stream logic) */
    scByte_t crcH = (crc >> 8) & 0xFF;
    scByte_t crcL = (crc & 0xFF);
    PUSH_STUFFED(crcH);
    PUSH_STUFFED(crcL);

    /* 4. Trailer */
    pOut[outIdx++] = SC_TRAILER_BYTE;

    __sc_exit();
    return SC_OKE;
}

scReturnVal_t scPushByte(scByte_t iByte) {
    /* 1. Write to Buffer */
    __scBuffer[__scHead] = iByte;
    
    /* 2. Advance Head */
    scSize_t nextHead = __scNextIdx(__scHead);
    
    /* 3. Handle Overwrite (Auto Delete) */
    /* Check if we are overwriting the Processing Tail */
    if (nextHead == __scProcTail) {
        __scProcTail = __scNextIdx(__scProcTail);
        /* If we are parsing, and we overwrote the start of the current parse, abort parse */
        if (__scParsingStartIdx != -1 && __scProcTail == __scNextIdx(__scParsingStartIdx)) {
            __scParsingStartIdx = -1; 
        }
    }

    /* Check if we are overwriting a Known Frame's Start Position */
    if (__scFrameCount > 0) {
        scSize_t oldestFrameStart = __scIDKnownPosList[__scFrameListTail] & 0x0FFF;
        /* If head hits the start of oldest frame, discard it */
        if (nextHead == oldestFrameStart) {
            __scDiscardOldestFrameMeta();
        }
    }

    __scHead = nextHead;

    #if (SC_MULTI_TASK_EN == 0)
    /* If Single Task, process immediately */
    scPooling();
    #endif

    return SC_OKE;
}

scReturnVal_t scPushBytes(void* iArr, scSize_t iSize) {
    if (!iArr) return SC_ERR_NULL;
    scByte_t* pData = (scByte_t*)iArr;
    for (int i = 0; i < iSize; i++) {
        scPushByte(pData[i]);
    }
    return SC_OKE;
}

void scPooling(void) {
    /* Process from __scProcTail to __scHead */
    while (__scProcTail != __scHead) {
        scByte_t b = __scBuffer[__scProcTail];
        
        if (b == SC_HEADER_BYTE) {
            /* Found Header: Mark start. If we were already parsing, discard old (sync to new) */
            __scParsingStartIdx = __scProcTail;
        }
        else if (b == SC_TRAILER_BYTE) {
            if (__scParsingStartIdx != -1) {
                /* Potential Frame Found: [Start ... Current] */
                /* Validation Phase (CRC & Unstuffing) */
                scSize_t checkIdx = __scNextIdx(__scParsingStartIdx);
                scWord_t runningCrc = 0xFFFF;
                scByte_t idByte = 0;
                int payloadLen = 0;
                int isValid = 1;
                
                /* Buffers for last 2 bytes (CRC) */
                scByte_t b0 = 0, b1 = 0; 

                while (checkIdx != __scProcTail) {
                    scByte_t val = __scBuffer[checkIdx];
                    
                    /* Unstuffing */
                    if (val == SC_ESC_BYTE) {
                        checkIdx = __scNextIdx(checkIdx);
                        /* Edge case: Trailer is immediately after ESC? Invalid */
                        if (checkIdx == __scProcTail) { isValid = 0; break; }
                        val = __scBuffer[checkIdx] ^ SC_XOR_MASK;
                    }

                    /* Capture ID */
                    if (payloadLen == 0) idByte = val;

                    /* CRC Calculation (Delay by 2 bytes) */
                    if (payloadLen >= 2) {
                        /* b0 is the byte pushed out of the 2-byte window, so it's data */
                        __scCRC16Cumpute(&runningCrc, b0, &runningCrc);
                    }
                    
                    /* Shift window */
                    b0 = b1;
                    b1 = val;
                    payloadLen++;
                    
                    checkIdx = __scNextIdx(checkIdx);
                }

                /* b0 = CRC_H, b1 = CRC_L */
                scWord_t receivedCrc = (b0 << 8) | b1;

                if (isValid && payloadLen >= 3 && runningCrc == receivedCrc) {
                    /* VALID FRAME */
                    if (__scFrameCount < SC_KNOWN_LIST_SIZE) {
                        /* Add to list */
                        __scIDKnownList[__scFrameListHead] = idByte;
                        __scIDKnownPosList[__scFrameListHead] = (scWord_t)(__scParsingStartIdx & 0x0FFF);
                        __scFrameListHead = (__scFrameListHead + 1) % SC_KNOWN_LIST_SIZE;
                        __scFrameCount++;
                    } else {
                        /* Full: Overwrite Oldest */
                        __scDiscardOldestFrameMeta();
                        __scIDKnownList[__scFrameListHead] = idByte;
                        __scIDKnownPosList[__scFrameListHead] = (scWord_t)(__scParsingStartIdx & 0x0FFF);
                        __scFrameListHead = (__scFrameListHead + 1) % SC_KNOWN_LIST_SIZE;
                        __scFrameCount++;
                    }
                } else {
                    __sc_err("SC: CRC Fail or Invalid Frame");
                }

                __scParsingStartIdx = -1; /* Reset */
            }
        }

        __scProcTail = __scNextIdx(__scProcTail);
    }
}

scReturnVal_t scHasNewFrame(void) {
    return (__scFrameCount > 0) ? SC_OKE : SC_ERR;
}

scSize_t scCountFrame(void) {
    return __scFrameCount;
}

/* --- Blocking Getters --- */

scReturnVal_t scGetLatestFrame(scByte_t* oHeader, scByte_t* oTrailer, scByte_t* oDataArr, scSize_t oArrSize, int32_t timeout) {
    if (__scWaitForFrame(timeout) != SC_OKE) return SC_ERR_TIMEOUT;
    
    scSize_t idx = (__scFrameListHead == 0) ? (SC_KNOWN_LIST_SIZE - 1) : (__scFrameListHead - 1);
    scSize_t startPos = __scIDKnownPosList[idx] & 0x0FFF;
    
    return __scExtractFrame(startPos, oHeader, oTrailer, oDataArr, oArrSize);
}

scReturnVal_t scGetOldestFrame(scByte_t* oHeader, scByte_t* oTrailer, scByte_t* oDataArr, scSize_t oArrSize, int32_t timeout) {
    if (__scWaitForFrame(timeout) != SC_OKE) return SC_ERR_TIMEOUT;

    scSize_t startPos = __scIDKnownPosList[__scFrameListTail] & 0x0FFF;
    return __scExtractFrame(startPos, oHeader, oTrailer, oDataArr, oArrSize);
}

scReturnVal_t scRemoveLatestFrame(scByte_t* oHeader, scByte_t* oTrailer, scByte_t* oArr, scSize_t oSize, int32_t timeout) {
    scReturnVal_t ret = scGetLatestFrame(oHeader, oTrailer, oArr, oSize, timeout);
    if (ret == SC_OKE) {
        __scFrameListHead = (__scFrameListHead == 0) ? (SC_KNOWN_LIST_SIZE - 1) : (__scFrameListHead - 1);
        __scFrameCount--;
    }
    return ret;
}

scReturnVal_t scRemoveOldestFrame(scByte_t* oHeader, scByte_t* oTrailer, scByte_t* oArr, scSize_t oSize, int32_t timeout) {
    scReturnVal_t ret = scGetOldestFrame(oHeader, oTrailer, oArr, oSize, timeout);
    if (ret == SC_OKE) {
        __scDiscardOldestFrameMeta();
    }
    return ret;
}

scReturnVal_t scGetDataByte(scByte_t* oArr, scSize_t oSize, int32_t timeout) {
    return scGetOldestFrame(NULL, NULL, oArr, oSize, timeout);
}

/* Wrappers / Legacy */
scReturnVal_t scGetHeaderByte(void* oHeaderByte) { 
    __sc_err("[scGetHeaderByte] This function is deprecated!");
    return SC_ERR; 
}
scReturnVal_t scGetTrailerByte(void* oTrailerByte) { 
    __sc_err("[scGetTrailerByte] This function is deprecated!");
    return SC_ERR; 
}

scReturnVal_t scDecode(void* iArr, scSize_t iSize, void* oArr, scSize_t oArrSize) {
    __sc_err("[scDecode] This function is deprecated!");
    /* Helper for manual decode - logic similar to extract frame */
    return SC_ERR; // Not implemented for circular buffer context
}

void scNewPoolingTask(void) {
    /* Platform dependent: Create a task that loops:
       while(1) {
           scPooling();
           scTaskDelay(5); // 5ms cycle
       }
    */
}

#endif /* SAFE_COM_EN */