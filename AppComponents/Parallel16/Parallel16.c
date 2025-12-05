#include "Parallel16.h"

/// @brief Allocates memory for a new Parallel16 device instance.
DefaultRet_t P16CreateDevice(P16Dev_t ** dev){
    P16Entry("P16CreateDevice(%p)", dev);
    P16NULLCheck(dev, "dev", "P16CreateDevice", goto returnERR_NULL;);

    /// Allocate memory for the struct, NOT the pointer
    (*dev) = (P16Dev_t*)malloc(sizeof(P16Dev_t));
    if ((*dev) == NULL) {
        P16Err("Malloc failed");
        goto returnERR_NULL; // Or ERR_MEM
    }

    /// Initialize memory to zero
    memset((*dev), 0, sizeof(P16Dev_t));
    
    #if (P16_MUTEX_EN == 1)
        /// Initialize Mutex if enabled
        (*dev)->mutex = CreateMutex();
    #endif

    P16ReturnWithLog(OKE, "P16CreateDevice() : OKE");

    returnERR_NULL:
        P16ReturnWithLog(ERR_NULL, "P16CreateDevice() : ERR_NULL");
}

/// @brief Configures the pins and calculates bitmasks for fast IO.
DefaultRet_t P16ConfigDevice(P16Dev_t * dev, P16DataPin_t* dataPin, P16ControlPin_t * controlPin){
    P16Entry("P16ConfigDevice(%p, %p, %p)", dev, dataPin, controlPin);
    P16NULLCheck(dev, "dev", "P16ConfigDevice", goto returnERR_NULL;);
    P16NULLCheck(dataPin, "dataPin", "P16ConfigDevice", goto returnERR_NULL;);
    P16NULLCheck(controlPin, "controlPin", "P16ConfigDevice", goto returnERR_NULL;);

    pin_t currentPin = 0;
    
    /// Copy Pin configuration to device struct (Crucial step missing in original code)
    dev->DatPin = *dataPin;
    dev->CtlPin = *controlPin;

    /// Reset masks
    (dev->DatPinMask)[16] = 0;
    (dev->CtlPinMask)[5]  = 0;
    
    /// Check and record data pin mask
    REP(i, 0, 16){
        currentPin = (dev->DatPin).arr[i]; // Use DatPin (PascalCase)
        if (__isnot_negative(currentPin) && currentPin <= 31) {
            (dev->DatPinMask)[i] = 0; // Clear first
            P16AddGPIO((dev->DatPinMask)[i], currentPin);
            P16AddGPIO((dev->DatPinMask)[16], currentPin); // Accumulate total mask
        } else {
            goto returnERR_INVALID_ARG;
        }
    }

    /// Check and record control pin mask
    REP(i, 0, 5){
        currentPin = (dev->CtlPin).arr[i]; // Use CtlPin (PascalCase)
        if (__isnot_negative(currentPin) && currentPin <= 31) {
            (dev->CtlPinMask)[i] = 0; // Clear first
            P16AddGPIO((dev->CtlPinMask)[i], currentPin);
            P16AddGPIO((dev->CtlPinMask)[5], currentPin); // Accumulate total mask
        } else {
            goto returnERR_INVALID_ARG;
        }
    }

    P16ReturnWithLog(OKE, "P16ConfigDevice() : OKE");
    returnERR_NULL:
        P16ReturnWithLog(ERR_NULL, "P16ConfigDevice() : ERR_NULL");
    returnERR_INVALID_ARG:
        P16ReturnWithLog(ERR_INVALID_ARG, "P16ConfigDevice() : ERR_INVALID_ARG");
}

/// @brief Sets up GPIO direction and initial states.
DefaultRet_t P16StartUpDevice(P16Dev_t * dev, P16DataPin_t* dataPin, P16ControlPin_t * controlPin) {
    P16Entry("P16StartUpDevice(...)");
    (void)dataPin; (void)controlPin; // Unused args warning suppression

    /// Config Control Pins as Output
    if(GPIOConfigOutputMask((dev->CtlPinMask)[5]) != OKE)    goto returnERR;
    
    /// Config Data Pins as Output initially (or Input depending on default state)
    if(GPIOConfigOutputMask((dev->DatPinMask)[16]) != OKE)   goto returnERR;

    P16SetHighResetPin(dev);
    P16SetLowRegSelPin(dev);
    P16SetHighReadPin(dev);
    P16SetHighWritePin(dev);
    P16SetHighChipSelPin(dev);

    P16Exit("P16StartUpDevice() : OKE");
    return OKE;
    returnERR:
        P16Exit("P16StartUpDevice() : ERR");
        return ERR;
}

/// @brief Switch data bus to Input mode.
DefaultRet_t P16SetModeRead(P16Dev_t * dev){
    P16Entry1("P16SetModeRead(%p)", dev);
    if(IsNull(dev)) P16ReturnWithLog1(ERR_NULL, "P16SetModeRead(): ERR_NULL");

    if(GPIOFastConfigInputMask(dev->DatPinMask[16]) != OKE)
        P16ReturnWithLog1(ERR, "P16SetModeRead(): ERR");

    P16SetHighReadPin(dev);
    P16SetHighWritePin(dev);
    
    P16ReturnWithLog1(OKE, "P16SetModeRead(): OKE");
}

/// @brief Switch data bus to Output mode.
DefaultRet_t P16SetModeWrite(P16Dev_t * dev){
    P16Entry1("P16SetModeWrite(%p)", dev);
    if(IsNull(dev)) P16ReturnWithLog1(ERR_NULL, "P16SetModeRead(): ERR_NULL");

    if(GPIOFastConfigOutputMask(dev->DatPinMask[16]) != OKE)
        P16ReturnWithLog1(ERR, "P16SetModeWrite(): ERR");

    P16SetHighReadPin(dev);
    P16SetHighWritePin(dev);
    
    P16ReturnWithLog1(OKE, "P16SetModeWrite(): OKE");
}

/// @brief Writes a single 16-bit word to the bus.
DefaultRet_t P16Write(P16Dev_t * dev, P16Data_t data16){
    P16Entry("P16Write(%p, 0x%04x)", dev, data16);
    
    if(IsNull(dev)) P16ReturnWithLog(ERR_NULL, "P16Write() : ERR_NULL");

    #if (P16_MUTEX_EN == 1)
    __enterCriticalSection(&(dev->mutex));
    #endif

    uint32_t GPIOSetMask = 0, GPIOClearMask = 0;

    /// Prepare gpio set/clear bit mask 
    REP(i, 0, P16_DATA_PIN_NUM) {
        if(data16 & Mask16(i)) 
            GPIOSetMask |= (dev->DatPinMask)[i];
        else 
            GPIOClearMask |= (dev->DatPinMask)[i];
    }

    /// Export data to output (Direct Register Access)
    GPIO.out_w1ts = GPIOSetMask;
    GPIO.out_w1tc = GPIOClearMask;

    /// Strobe Write Pin
    P16SetLowWritePin(dev);
    P16DelayHalfPeriod();
    P16SetHighWritePin(dev);
    P16DelayHalfPeriod();

    #if (P16_MUTEX_EN == 1)
    __exitCriticalSection(&(dev->mutex));
    #endif

    P16ReturnWithLog(OKE, "P16Write() : OKE");
}

/// @brief Reads a single 16-bit word from the bus.
DefaultRet_t P16Read(P16Dev_t * dev, P16Data_t * data16){
    P16Entry("P16Read(%p, %p)", dev, data16);
    
    if(IsNull(dev)) P16ReturnWithLog(ERR_NULL, "P16Read() : ERR_NULL");

    #if (P16_MUTEX_EN == 1)
    __enterCriticalSection(&(dev->mutex));
    #endif

    /// Strobe Read Pin
    P16SetLowReadPin(dev);
    P16DelayHalfPeriod();
    
    /// Snapshot input register
    uint32_t currentGPIO = GPIO.in;
    
    P16SetHighReadPin(dev);
    P16DelayHalfPeriod();

    /// Reconstruct 16-bit data from scattered pins
    *data16 = 0;
    /// Optimized unrolled loop using the DatPin struct
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__0 ) & 1) << 0;
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__1 ) & 1) << 1;
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__2 ) & 1) << 2;
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__3 ) & 1) << 3;
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__4 ) & 1) << 4;
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__5 ) & 1) << 5;
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__6 ) & 1) << 6;
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__7 ) & 1) << 7;
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__8 ) & 1) << 8;
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__9 ) & 1) << 9;
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__10) & 1) << 10;
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__11) & 1) << 11;
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__12) & 1) << 12;
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__13) & 1) << 13;
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__14) & 1) << 14;
    *data16 |= ((currentGPIO >> dev->DatPin.pin.__15) & 1) << 15;

    #if (P16_MUTEX_EN == 1)
    __exitCriticalSection(&(dev->mutex));
    #endif

    P16ReturnWithLog(OKE, "P16Read() : OKE");
}

/// @brief Writes an array of 16-bit words.
DefaultRet_t P16WriteArray(P16Dev_t * dev, const P16Data_t *data16, P16Size_t dataSize){
    // Fix: Correct Log Macro Name
    P16Entry("P16WriteArray(%p, %p, %d)", dev, data16, dataSize); 
    
    if(IsNull(dev) || IsNull(data16)) P16ReturnWithLog(ERR_NULL, "ERR_NULL");

    #if (P16_MUTEX_EN == 1)
    __enterCriticalSection(&(dev->mutex));
    #endif

    REP(i, 0, dataSize){
        uint32_t GPIOSetMask = 0, GPIOClearMask = 0;

        REP(k, 0, P16_DATA_PIN_NUM) {
            if(data16[i] & Mask16(k)) 
                GPIOSetMask |= (dev->DatPinMask)[k];
            else 
                GPIOClearMask |= (dev->DatPinMask)[k];
        }
        
        GPIO.out_w1ts = GPIOSetMask;
        GPIO.out_w1tc = GPIOClearMask;

        P16SetLowWritePin(dev);
        P16DelayHalfPeriod();
        P16SetHighWritePin(dev);
        P16DelayHalfPeriod();
    }

    #if (P16_MUTEX_EN == 1)
    __exitCriticalSection(&(dev->mutex));
    #endif
    
    P16ReturnWithLog(OKE, "P16WriteArray() : OKE");
}

/// @brief Reads multiple 16-bit words.
DefaultRet_t P16ReadArray(P16Dev_t * dev, P16Data_t * data16, P16Size_t dataSize){
    P16Entry("P16ReadArray(%p, %p, %d)", dev, data16, dataSize);

    #if (P16_MUTEX_EN == 1)
    __enterCriticalSection(&(dev->mutex));
    #endif

    REP(i, 0, dataSize){
        P16SetLowReadPin(dev);
        P16DelayHalfPeriod();
        
        uint32_t currentGPIO = GPIO.in;
        
        P16SetHighReadPin(dev);
        P16DelayHalfPeriod();
        
        // Reconstruct data
        data16[i] = 0;
        REP(bit, 0, 16) {
            // Check if input matches the mask for this bit position
            if (currentGPIO & (dev->DatPinMask)[bit]) {
                data16[i] |= (1 << bit);
            }
        }
    }

    #if (P16_MUTEX_EN == 1)
    __exitCriticalSection(&(dev->mutex));
    #endif

    P16ReturnWithLog(OKE, "P16ReadArray() : OKE");
}