/**
 * @file P16Com.c
 * @brief 16-bit Parallel Communication Driver Implementation
 * @details Implements "Safe-Mode" bus handling: Data bus is always kept as INPUT 
 * unless a WRITE operation is strictly performing.
 * @author Nguyen Thanh Phu
 */

#include "P16Com.h"

/// @brief Allocates a new P16ComPin_t object
P16ComPin_t * P16ComNew(){
    /// Allocate memory
    P16ComPin_t * devPtr = (P16ComPin_t *) malloc(sizeof(P16ComPin_t));
    
    if (devPtr == NULL) {
        P16Err("P16ComNew: Malloc failed!");
        return NULL;
    }

    /// Preset Ctl/Dat pin arrays to -1 (Invalid/Unused)
    REPN(i, P16COM_DAT_PIN_NUM_W_PADDING){
        devPtr->DatPinArr[i] = -1;
    }
    REPN(i, P16COM_CTL_PIN_NUM_W_PADDING){
        devPtr->CtlPinArr[i] = -1;
    }
    /// Preset status to uninitialized
    devPtr->StatusFlag = 0;
    
    return devPtr;
}

/// @brief Initialize the driver GPIOs and Flags
DefaultRet_t P16ComInit(P16ComPin_t * Dev){
    P16Entry("P16ComInit(%p)", Dev);
    
    if(Dev == NULL) {
        P16ReturnWithLog(STAT_ERR_NULL, "P16ComInit: Device pointer is NULL");
    }

    uint64_t IOMask = 0;
    
    /// 1. Loop for DATA pins configuration
    REPN(i, P16COM_DAT_PIN_NUM){
        if(IsStandardPin(Dev->DatPinArr[i])){
            IOMask |= Mask64(Dev->DatPinArr[i]);
        }else{
            P16Err("[P16ComInit] Data pin index %d (GPIO %d) is not a Standard pin!", i, Dev->DatPinArr[i]);
            P16ReturnWithLog(STAT_ERR_INIT_FAILED, "P16ComInit: Invalid Data Pin");
        }
    }
    /// @note SAFETY: Default state of Data Bus is INPUT (High Impedance)
    IOConfigAsInput(IOMask, -1, -1); 

    IOMask = 0;
    /// 2. Loop for CONTROL pins configuration
    REPN(i, P16COM_CTL_PIN_NUM){
        if(IsStandardPin(Dev->CtlPinArr[i])){
            IOMask |= Mask64(Dev->CtlPinArr[i]);
        }else{
            P16Err("[P16ComInit] Control pin index %d (GPIO %d) is not a Standard pin!", i, Dev->CtlPinArr[i]);
            P16ReturnWithLog(STAT_ERR_INIT_FAILED, "P16ComInit: Invalid Control Pin");
        }
    }
    /// Set Control pins to OUTPUT (Push-Pull)
    IOConfigAsOutput(IOMask, -1, -1);
    
    /// Set Control pins to IDLE state (High for active-low signals)
    IOStandardSet(IOMask);

    /// 3. Mark as initialized 
    Dev->StatusFlag |= P16COM_INITIALIZED;
    
    P16ReturnWithLog(STAT_OKE, "P16ComInit: STAT_OKE");
}

/// @brief Generate a hard reset pulse
void P16ComMakeReset(P16ComPin_t * Dev){
    #if (P16COM_INIT_CHECK_EN == 1)
        if( !((Dev->StatusFlag) & P16COM_INITIALIZED) ){
            P16Err("P16ComMakeReset: Device not initialized!");
            return;
        }
    #endif

    /// Pulse Reset: Low -> Delay -> High
    P16SetLowResetPin(Dev);
    P16BlockingDelay(P16HalfClockCycle);
    P16SetHighResetPin(Dev);
    P16BlockingDelay(P16HalfClockCycle);
}

/// @brief Write a single word to the bus
void P16ComWrite(P16ComPin_t * Dev, P16Data_t Data){

    #if (P16COM_INIT_CHECK_EN == 1)
        if( !((Dev->StatusFlag) & P16COM_INITIALIZED) ){
            P16Err("P16ComWrite: Device not initialized!");
            return;
        }
    #endif

    /// 1. Prepare Mask for Data Pins
    uint64_t dataMask = 0;
    REPN(i, 16) { dataMask |= Mask64(Dev->DatPinArr[i]); }

    /// 2. SWITCH TO OUTPUT (Push-Pull) just for writing
    IOConfigAsOutput(dataMask, -1, -1);

    /// 3. Calculate Bit Masks
    uint32_t MaskSet = 0, MaskClr = 0;
    REPN(i, 16){
        (Data & (1 << i)) ? 
        (MaskSet |= Mask32(Dev->DatPinArr[i])) :
        (MaskClr |= Mask32(Dev->DatPinArr[i])) ;
    }

    /// 4. Drive Data to Pins
    IOStandardSet(MaskSet);
    IOStandardClr(MaskClr);

    /// 5. Strobe Write (Low -> High)
    P16SetLowWritePin(Dev);
    P16BlockingDelay(P16HalfClockCycle);
    P16SetHighWritePin(Dev);
    P16BlockingDelay(P16HalfClockCycle);

    /// 6. SAFETY: Switch back to INPUT (High-Z) immediately
    IOConfigAsInput(dataMask, -1, -1);
}

/// @brief Write a block of data to the bus (Burst Write)
void P16ComWriteArray(P16ComPin_t * Dev, P16Data_t * DataArr, P16Size_t Size){

    P16Entry("P16ComWriteArray(%p, %p, %d)", Dev, DataArr, Size);

    #if (P16COM_INIT_CHECK_EN == 1)
        if( !((Dev->StatusFlag) & P16COM_INITIALIZED) ){
            P16Err("P16ComWriteArray: Device not initialized!");
            return;
        }
    #endif

    if(IsNull(DataArr) || IsNotPos(Size)){
        P16Err("DataArr is NULL or Size not valid!");
        return;
    }

    /// 1. Prepare Mask
    uint64_t dataMask = 0;
    REPN(i, 16) { dataMask |= Mask64(Dev->DatPinArr[i]); }

    /// 2. SWITCH TO OUTPUT (Push-Pull) ONCE for the whole burst
    IOConfigAsOutput(dataMask, -1, -1);

    REPN(j, Size){
        P16Data_t currentData = DataArr[j];
        uint32_t MaskSet = 0, MaskClr = 0;

        /// Map bits
        REPN(i, 16){
            (currentData & (1 << i)) ? 
            (MaskSet |= Mask32(Dev->DatPinArr[i])) :
            (MaskClr |= Mask32(Dev->DatPinArr[i])) ;
        }

        /// Drive Data
        IOStandardSet(MaskSet);
        IOStandardClr(MaskClr);

        /// Strobe Write
        P16SetLowWritePin(Dev);
        P16BlockingDelay(P16HalfClockCycle);
        P16SetHighWritePin(Dev);
        P16BlockingDelay(P16HalfClockCycle);
    }
    
    /// 3. SAFETY: Switch back to INPUT (High-Z) after burst finishes
    IOConfigAsInput(dataMask, -1, -1);

    P16Exit("P16ComWriteArray: Done");
}

/// @brief Read a single word from the bus
P16Data_t P16ComRead(P16ComPin_t * Dev){
    #if (P16COM_INIT_CHECK_EN == 1)
        if( !((Dev->StatusFlag) & P16COM_INITIALIZED) ){
            P16Err("P16ComRead: Device not initialized!");
            return 0;
        }
    #endif

    /// @note Pins are already INPUT by default/after write, 
    /// but ensuring it here is safe practice (or can be removed for speed).
    /* uint64_t dataMask = 0;
    REPN(i, 16) { dataMask |= Mask64(Dev->DatPinArr[i]); }
    IOConfigAsInput(dataMask, -1, -1);
    */

    /// 1. Activate Read Strobe
    P16SetLowReadPin(Dev);
    P16BlockingDelay(P16HalfClockCycle);

    /// 2. Sample Data
    uint64_t portState = IOStandardGet();
    uint16_t result = 0;
    
    REPN(i, 16){
        if ( (portState >> Dev->DatPinArr[i]) & 0x1 ) {
            result |= (1 << i);
        }
    }

    /// 3. Deactivate Read Strobe
    P16SetHighReadPin(Dev);
    P16BlockingDelay(P16HalfClockCycle);

    return result;
}

/// @brief Read a block of data from the bus (Burst Read)
void P16ComReadArray(P16ComPin_t * Dev, P16Data_t * pBuff, P16Size_t Size){
    P16Entry("P16ComReadArray(%p, %p, %d)", Dev, pBuff, Size);

    #if (P16COM_INIT_CHECK_EN == 1)
        if( !((Dev->StatusFlag) & P16COM_INITIALIZED) ){
            P16Err("P16ComReadArray: Device not initialized!");
            return;
        }
    #endif

    if(IsNull(pBuff) || IsNotPos(Size)){
        P16Err("Buffer is NULL or Size not valid!");
        return;
    }

    /// @note Pins are assumed to be INPUT (Safe state)

    REPN(j, Size){
        P16SetLowReadPin(Dev);
        P16BlockingDelay(P16HalfClockCycle);

        uint64_t portState = IOStandardGet();
        uint16_t val = 0;
        REPN(i, 16){
            if ( (portState >> Dev->DatPinArr[i]) & 0x1 ) {
                val |= (1 << i);
            }
        }
        pBuff[j] = val;

        P16SetHighReadPin(Dev);
        P16BlockingDelay(P16HalfClockCycle);
    }
    
    P16Exit("P16ComReadArray: Done");
}