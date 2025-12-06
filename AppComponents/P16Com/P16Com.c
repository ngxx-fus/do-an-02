/**
 * @file P16Com.c
 * @brief 16-bit Parallel Communication Driver Implementation
 * @author Nguyen Thanh Phu
 */

#include "P16Com.h"

/// @brief Allocates a new P16Dev_t object
P16Dev_t * P16ComNew(){
    /// Allocate memory
    P16Dev_t * devPtr = (P16Dev_t *) malloc(sizeof(P16Dev_t));
    
    if (devPtr == NULL) {
        P16Err("[P16ComNew] Malloc failed!");
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
    devPtr->CtlIOMask = 0;
    devPtr->DatIOMask = 0;
    
    return devPtr;
}

/// @brief Deallocate memory for P16Dev_t object
void P16Delete(P16Dev_t * Dev){
    if(Dev != NULL){
        free(Dev);
    }
}

/// @brief Configure Control pins mapping and calculate IO Masks
DefaultRet_t P16ComConfigCtl(P16Dev_t * Dev, const Pin_t * CtlPins){
    P16Entry("P16ComConfigCtl(%p, %p)", Dev, CtlPins);

    if(IsNull(Dev) || IsNull(CtlPins)){
        P16ReturnWithLog(STAT_ERR_NULL, "P16ComConfigCtl() : STAT_ERR_NULL");
    }

    /// Configure and Mask Control Pins
    Dev->CtlIOMask = 0;
    REPN(i, P16COM_CTL_PIN_NUM){
        if(IsStandardPin(CtlPins[i])){
            Dev->CtlPinArr[i] = CtlPins[i];
            Dev->CtlIOMask |= Mask64(CtlPins[i]);
        } else {
            P16Err("[P16ComConfigCtl] Invalid Control Pin at index %d", i);
            P16ReturnWithLog(STAT_ERR_INVALID_ARG, "P16ComConfigCtl() : STAT_ERR_INVALID_ARG");
        }
    }
    
    P16ReturnWithLog(STAT_OKE, "P16ComConfigCtl() : STAT_OKE");
}

/// @brief (Private) Builds the lookup tables for GPIO masks to accelerate write operations.
/// @param Dev (P16Dev_t *) Pointer to the P16Dev_t object.
static void __P16ComBuildLuts(P16Dev_t * Dev) {
    P16Log("[__P16ComBuildLuts] Building mask lookup tables...");
    
    // Low byte LUT for D0-D7
    for (uint32_t i = 0; i < 256; i++) {
        uint64_t set_mask = 0;
        uint64_t clr_mask = 0;
        for (uint32_t bit = 0; bit < 8; bit++) {
            if ((i >> bit) & 1) {
                set_mask |= (1ULL << Dev->DatPinArr[bit]);
            } else {
                clr_mask |= (1ULL << Dev->DatPinArr[bit]);
            }
        }
        Dev->LowByteLut[i].set = set_mask;
        Dev->LowByteLut[i].clr = clr_mask;
    }

    // High byte LUT for D8-D15
    for (uint32_t i = 0; i < 256; i++) {
        uint64_t set_mask = 0;
        uint64_t clr_mask = 0;
        for (uint32_t bit = 0; bit < 8; bit++) {
            if ((i >> bit) & 1) {
                set_mask |= (1ULL << Dev->DatPinArr[bit + 8]);
            } else {
                clr_mask |= (1ULL << Dev->DatPinArr[bit + 8]);
            }
        }
        Dev->HighByteLut[i].set = set_mask;
        Dev->HighByteLut[i].clr = clr_mask;
    }
    P16Log("[__P16ComBuildLuts] LUTs built successfully.");
}

/// @brief Configure Data pins mapping and calculate IO Masks
DefaultRet_t P16ComConfigDat(P16Dev_t * Dev, const Pin_t * DatPins){
    P16Entry("P16ComConfigDat(%p, %p)", Dev, DatPins);

    if(IsNull(Dev) || IsNull(DatPins)){
        P16ReturnWithLog(STAT_ERR_NULL, "P16ComConfigDat() : STAT_ERR_NULL");
    }

    /// Configure and Mask Data Pins
    Dev->DatIOMask = 0;
    REPN(i, P16COM_DAT_PIN_NUM){
        if(IsStandardPin(DatPins[i])){
            Dev->DatPinArr[i] = DatPins[i];
            Dev->DatIOMask |= Mask64(DatPins[i]);
        } else {
            P16Err("[P16ComConfigDat] Invalid Data Pin at index %d", i);
            P16ReturnWithLog(STAT_ERR_INVALID_ARG, "P16ComConfigDat() : STAT_ERR_INVALID_ARG");
        }
    }

    // Build lookup tables for fast writes
    __P16ComBuildLuts(Dev);

    P16ReturnWithLog(STAT_OKE, "P16ComConfigDat() : STAT_OKE");
}

/// @brief Initialize the driver GPIOs and Flags
DefaultRet_t P16ComInit(P16Dev_t * Dev){
    P16Entry("P16ComInit(%p)", Dev);
    
    if(IsNull(Dev)) {
        P16ReturnWithLog(STAT_ERR_NULL, "P16ComInit() : STAT_ERR_NULL");
    }

    /// 1. Configure DATA pins
    /// @note Use pre-calculated DatIOMask for speed and sync
    if(Dev->DatIOMask == 0){
        P16Err("[P16ComInit] DatIOMask is empty! Run Config first?");
        P16ReturnWithLog(STAT_ERR_INIT_FAILED, "P16ComInit() : STAT_ERR_INIT_FAILED");
    }

    #if (P16COM_DB_NORMAL_OUTPUT_EN == 1)
        /// @note Normal state is OUTPUT (optimized for write)
        IOConfigAsOutput(Dev->DatIOMask, -1, -1);
    #else
        /// @note Normal state is INPUT (Safe Mode / High-Z)
        IOConfigAsInput(Dev->DatIOMask, -1, -1);
    #endif

    /// 2. Configure CONTROL pins
    if(Dev->CtlIOMask == 0){
        P16Err("[P16ComInit] CtlIOMask is empty! Run Config first?");
        P16ReturnWithLog(STAT_ERR_INIT_FAILED, "P16ComInit() : STAT_ERR_INIT_FAILED");
    }

    /// Set Control pins to OUTPUT (Push-Pull)
    IOConfigAsOutput(Dev->CtlIOMask, -1, -1);
    
    /// Set Control pins to IDLE state (High for active-low signals)
    IOStandardSet(Dev->CtlIOMask);

    /// 3. Mark as initialized 
    Dev->StatusFlag |= P16COM_INITIALIZED;
    
    P16ReturnWithLog(STAT_OKE, "P16ComInit() : STAT_OKE");
}

/// @brief Re-initializes the driver based on existing configuration
DefaultRet_t P16ComReConfig(P16Dev_t * Dev){
    P16Entry("P16ComReConfig(%p)", Dev);
    return P16ComInit(Dev);
}

/// @brief Generate a hard reset pulse
void P16ComMakeReset(P16Dev_t * Dev){
    #if (P16COM_INIT_CHECK_EN == 1)
        if( !((Dev->StatusFlag) & P16COM_INITIALIZED) ){
            P16Err("[P16ComMakeReset] Device not initialized!");
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
void P16ComWrite(P16Dev_t * Dev, P16Data_t Data){

    #if (P16COM_INIT_CHECK_EN == 1)
        if( !((Dev->StatusFlag) & P16COM_INITIALIZED) ){
            P16Err("[P16ComWrite] Device not initialized!");
            return;
        }
    #endif

    #if (P16COM_DB_NORMAL_OUTPUT_EN == 0)
        /// Switch to OUTPUT using pre-calculated mask
        IOConfigAsOutput(Dev->DatIOMask, -1, -1);
    #endif

    /// Calculate Bit Masks
    uint32_t MaskSet = 0, MaskClr = 0;
    REPN(i, 16){
        (Data & (1 << i)) ? 
        (MaskSet |= Mask32(Dev->DatPinArr[i])) :
        (MaskClr |= Mask32(Dev->DatPinArr[i])) ;
    }
    /// Use pre-calculated LUTs for high-speed mask generation
    uint8_t low_byte = Data & 0xFF;
    uint8_t high_byte = (Data >> 8) & 0xFF;

    uint64_t MaskSet = Dev->LowByteLut[low_byte].set | Dev->HighByteLut[high_byte].set;
    uint64_t MaskClr = Dev->LowByteLut[low_byte].clr | Dev->HighByteLut[high_byte].clr;

    /// Drive Data to Pins
    IOStandardSet(MaskSet);
    IOStandardClr(MaskClr);

    /// Strobe Write
    P16MakeWritePulse(Dev);

    #if (P16COM_DB_NORMAL_OUTPUT_EN == 0)
        /// Switch back to INPUT (Safe Mode)
        IOConfigAsInput(Dev->DatIOMask, -1, -1);
    #endif
}

/// @brief Write a block of data to the bus (Burst Write)
void P16ComWriteArray(P16Dev_t * Dev, P16Data_t * DataArr, P16Size_t Size){

    P16Entry("P16ComWriteArray(%p, %p, %d)", Dev, DataArr, Size);

    #if (P16COM_INIT_CHECK_EN == 1)
        if( !((Dev->StatusFlag) & P16COM_INITIALIZED) ){
            P16Err("[P16ComWriteArray] Device not initialized!");
            return;
        }
    #endif

    if(IsNull(DataArr) || IsNotPos(Size)){
        P16Err("[P16ComWriteArray] DataArr is NULL or Size not valid!");
        return;
    }

    #if (P16COM_DB_NORMAL_OUTPUT_EN == 0)
        /// Switch to OUTPUT ONCE for the whole burst
        IOConfigAsOutput(Dev->DatIOMask, -1, -1);
    #endif

    uint8_t low_byte, high_byte;
    uint64_t MaskSet, MaskClr;

    REPN(j, Size){
        P16Data_t currentData = DataArr[j];
        uint32_t MaskSet = 0, MaskClr = 0;
        
        /// Use pre-calculated LUTs for high-speed mask generation
        low_byte = currentData & 0xFF;
        high_byte = (currentData >> 8) & 0xFF;
        MaskSet = Dev->LowByteLut[low_byte].set | Dev->HighByteLut[high_byte].set;
        MaskClr = Dev->LowByteLut[low_byte].clr | Dev->HighByteLut[high_byte].clr;

        /// Map bits
        REPN(i, 16){
            (currentData & (1 << i)) ? 
            (MaskSet |= Mask32(Dev->DatPinArr[i])) :
            (MaskClr |= Mask32(Dev->DatPinArr[i])) ;
        }

        /// Drive Data
        /// Drive Data to Pins
        IOStandardSet(MaskSet);
        IOStandardClr(MaskClr);

        /// Strobe Write
        P16MakeWritePulse(Dev);
    }
    
    #if (P16COM_DB_NORMAL_OUTPUT_EN == 0)
        /// Switch back to INPUT
        IOConfigAsInput(Dev->DatIOMask, -1, -1);
    #endif

    P16Exit("P16ComWriteArray() : Done");
}

/// @brief Read a single word from the bus
P16Data_t P16ComRead(P16Dev_t * Dev){
    #if (P16COM_INIT_CHECK_EN == 1)
        if( !((Dev->StatusFlag) & P16COM_INITIALIZED) ){
            P16Err("[P16ComRead] Device not initialized!");
            return 0;
        }
    #endif

    #if (P16COM_DB_NORMAL_OUTPUT_EN == 1)
        /// Switch Data Bus to INPUT for reading
        IOConfigAsInput(Dev->DatIOMask, -1, -1);
    #endif

    /// Activate Read Strobe
    P16SetLowReadPin(Dev);
    P16BlockingDelay(P16HalfClockCycle);

    /// Sample Data
    uint64_t portState = IOStandardGet();
    uint16_t result = 0;
    
    REPN(i, 16){
        if ( (portState >> Dev->DatPinArr[i]) & 0x1 ) {
            result |= (1 << i);
        }
    }

    /// Deactivate Read Strobe
    P16SetHighReadPin(Dev);
    P16BlockingDelay(P16HalfClockCycle);

    #if (P16COM_DB_NORMAL_OUTPUT_EN == 1)
        /// Switch back to OUTPUT (Normal State)
        IOConfigAsOutput(Dev->DatIOMask, -1, -1);
    #endif

    return result;
}

/// @brief Read a block of data from the bus (Burst Read)
void P16ComReadArray(P16Dev_t * Dev, P16Data_t * pBuff, P16Size_t Size){
    P16Entry("P16ComReadArray(%p, %p, %d)", Dev, pBuff, Size);

    #if (P16COM_INIT_CHECK_EN == 1)
        if( !((Dev->StatusFlag) & P16COM_INITIALIZED) ){
            P16Err("[P16ComReadArray] Device not initialized!");
            return;
        }
    #endif

    if(IsNull(pBuff) || IsNotPos(Size)){
        P16Err("[P16ComReadArray] Buffer is NULL or Size not valid!");
        return;
    }

    #if (P16COM_DB_NORMAL_OUTPUT_EN == 1)
        /// Switch Data Bus to INPUT for reading
        IOConfigAsInput(Dev->DatIOMask, -1, -1);
    #endif

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
    
    #if (P16COM_DB_NORMAL_OUTPUT_EN == 1)
        /// Switch back to OUTPUT
        IOConfigAsOutput(Dev->DatIOMask, -1, -1);
    #endif

    P16Exit("P16ComReadArray() : Done");
}