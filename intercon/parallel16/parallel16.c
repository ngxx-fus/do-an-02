#include "parallel16.h"

def p16CreateDevice(parallel16Dev_t ** dev){
    __p16Entry("p16CreateDevice(%p)", dev);
    __p16NULLCheck(dev, "dev", "p16CreateDevice", goto returnERR_NULL;);

    (*dev) = malloc(sizeof(parallel16Dev_t));

    memset((*dev), 0, sizeof(*dev));

    __p16ReturnWithLog(OKE, "p16CreateDevice() : OKE");

    returnERR_NULL:
        __p16ReturnWithLog(ERR_NULL, "p16CreateDevice() : ERR_NULL");
        
}

def p16ConfigDevice(parallel16Dev_t * dev, p16DataPin_t* dataPin, p16ControlPin_t * controlPin){
    __p16Entry("p16ConfigDevice(%p, %p, %p)", dev, dataPin, controlPin);
    __p16NULLCheck(dev, "dev", "p16ConfigDevice", goto returnERR_NULL;);
    __p16NULLCheck(dataPin, "dataPin", "p16ConfigDevice", goto returnERR_NULL;);
    __p16NULLCheck(controlPin, "controlPin", "p16ConfigDevice", goto returnERR_NULL;);

    pin_t currentPin            = 0;
    (dev->dataPinMask)[16]      = 0;
    (dev->controlPinMask)[16]   = 0;
    
    /// Check and record data pin
    REP(i, 0, 16){
        currentPin = (dev->dataPin).arr[i];
        if (0 <= currentPin && currentPin <= 31) {
            (dev->dataPinMask)[i] |= __mask32(currentPin);
            (dev->dataPinMask)[16] |= __mask32(currentPin);

        } else {
            goto returnERR_INVALID_ARG;
        }
    }

    /// Check and record control pin
    REP(i, 0, 4){
        currentPin = (dev->controlPin).arr[i];
        if (0 <= currentPin && currentPin <= 31) {
            (dev->controlPinMask)[i] |= __mask32(currentPin);
            (dev->controlPinMask)[5] |= __mask32(currentPin);
        } else {
            goto returnERR_INVALID_ARG;
        }
    }

    __p16ReturnWithLog(OKE, "p16ConfigDevice() : OKE");
    returnERR_NULL:
        __p16ReturnWithLog(ERR_NULL, "p16ConfigDevice() : ERR_NULL");
    returnERR_INVALID_ARG:
        __p16ReturnWithLog(ERR_INVALID_ARG, "p16ConfigDevice() : ERR_INVALID_ARG");
        
}

def p16StartUpDevice(parallel16Dev_t * dev, p16DataPin_t* dataPin, p16ControlPin_t * controlPin) {
    __p16Entry("p16StartUpDevice(...)");

    if(GPIOConfigOutputMask((dev->controlPinMask)[16]) != OKE)    goto returnERR;
    if(GPIOConfigOutputMask((dev->dataPinMask)[5]) != OKE)       goto returnERR;

    __p16SetHighResetPin(dev);
    __p16SetLowRegSelPin(dev);
    __p16SetHighReadPin(dev);
    __p16SetHighWritePin(dev);
    __p16SetHighChipSelPin(dev);

    __p16Exit("p16StartUpDevice() : OKE");
    return OKE;
    returnERR:
        __p16Exit("p16StartUpDevice() : ERR");
        return ERR;
}

def p16SetModeRead(parallel16Dev_t * dev){
    __p16Entry1("p16SetModeRead(%p)", dev);
    if(__is_null(dev))
        __p16ReturnWithLog1(ERR_NULL, "p16SetModeRead(): ERR_NULL");

    if(GPIOFastConfigInputMask(dev->dataPinMask) != OKE)
        __p16ReturnWithLog1(ERR, "p16SetModeRead(): ERR");

    __p16SetHighReadPin(dev);
    __p16SetHighWritePin(dev);
    
    __p16ReturnWithLog1(OKE, "p16SetModeRead(): OKE");
}

def p16SetModeWrite(parallel16Dev_t * dev){
    __p16Entry1("p16SetModeWrite(%p)", dev);
    if(__is_null(dev))
        __p16ReturnWithLog1(ERR_NULL, "p16SetModeRead(): ERR_NULL");

    if(GPIOFastConfigOutputMask(dev->dataPinMask) != OKE)
        __p16ReturnWithLog1(ERR, "p16SetModeWrite(): ERR");

    __p16SetHighReadPin(dev);
    __p16SetHighWritePin(dev);
    
    __p16ReturnWithLog1(OKE, "p16SetModeWrite(): OKE");
}

def p16Write(parallel16Dev_t * dev, p16Data_t data16){
    __p16Entry("p16Write(%p, 0x%04x)", dev, data16);

    __enterCriticalSection(&(dev->mutex));

    if(__is_null(dev)) goto returnERR_NULL;

    uint32_t GPIOSetMask = 0, GPIOClearMask = 0;

    /// Prepair gpio set/clear bit mask 
    REP(i, 0, P16_DATA_PIN_NUM) 
        if(data16 & __mask16(i)) 
            GPIOSetMask |= (dev->dataPinMask)[i];
        else 
            GPIOClearMask |= (dev->dataPinMask)[i];
    /// Export data to output
    GPIO.out_w1ts = GPIOSetMask;
    GPIO.out_w1tc = GPIOClearMask;

    /// Make falling edge of WRITE pin
    __p16SetLowWritePin(dev);
    __p16DelayHalfPeriod();
    /// Make raising edge of WRITE pin
    __p16SetHighWritePin(dev);
    __p16DelayHalfPeriod();

    __exitCriticalSection(&(dev->mutex));
    __p16ReturnWithLog(OKE, "p16Write() : OKE");
    returnERR:
        __exitCriticalSection(&(dev->mutex));
        __p16ReturnWithLog(ERR, "p16Write() : ERR");
    returnERR_NULL:
        __exitCriticalSection(&(dev->mutex));
        __p16ReturnWithLog(ERR_NULL, "p16Write() : ERR_NULL");
}

def p16Read(parallel16Dev_t * dev, p16Data_t * data16){
    __p16Entry("p16Read(%p, %p)", dev, data16);

    __enterCriticalSection(&(dev->mutex));

    if(__is_null(dev)) goto returnERR_NULL;

    /// Make falling edge of READ pin
    __p16SetLowReadPin(dev);
    __p16DelayHalfPeriod();
    /// Make raising edge of READ pin
    __p16SetHighReadPin(dev);
    __p16DelayHalfPeriod();

    /// Reset (*data16)
    (*data16) = 0;

    /// Snapshot input register once
    uint32_t currentGPIO = GPIO.in;

    /// Read data from 16 data pins
    *data16 =
        ((currentGPIO >> dev->dataPin.pin.__0 ) & 1) << 0  |
        ((currentGPIO >> dev->dataPin.pin.__1 ) & 1) << 1  |
        ((currentGPIO >> dev->dataPin.pin.__2 ) & 1) << 2  |
        ((currentGPIO >> dev->dataPin.pin.__3 ) & 1) << 3  |
        ((currentGPIO >> dev->dataPin.pin.__4 ) & 1) << 4  |
        ((currentGPIO >> dev->dataPin.pin.__5 ) & 1) << 5  |
        ((currentGPIO >> dev->dataPin.pin.__6 ) & 1) << 6  |
        ((currentGPIO >> dev->dataPin.pin.__7 ) & 1) << 7  |
        ((currentGPIO >> dev->dataPin.pin.__8 ) & 1) << 8  |
        ((currentGPIO >> dev->dataPin.pin.__9 ) & 1) << 9  |
        ((currentGPIO >> dev->dataPin.pin.__10) & 1) << 10 |
        ((currentGPIO >> dev->dataPin.pin.__11) & 1) << 11 |
        ((currentGPIO >> dev->dataPin.pin.__12) & 1) << 12 |
        ((currentGPIO >> dev->dataPin.pin.__13) & 1) << 13 |
        ((currentGPIO >> dev->dataPin.pin.__14) & 1) << 14 |
        ((currentGPIO >> dev->dataPin.pin.__15) & 1) << 15;

    __exitCriticalSection(&(dev->mutex));
    __p16ReturnWithLog(OKE, "p16Read() : OKE");
    returnERR_NULL:
        __exitCriticalSection(&(dev->mutex));
        __p16ReturnWithLog(ERR_NULL, "p16Read() : ERR_NULL");
}

def p16WriteArray(parallel16Dev_t * dev, const p16Data_t *data16, p16Size_t dataSize){
    p16WriteArray("p16Write(%p, %p, %d)", dev, data16, dataSize);
    __enterCriticalSection(&(dev->mutex));

    if(__is_null(dev) || __is_null(data16)) goto returnERR_NULL;


    /// Send each byte
    REP(i, 0, dataSize){
        uint32_t GPIOSetMask = 0, GPIOClearMask = 0;

        /// Prepair gpio set/clear bit mask 
        REP(k, 0, P16_DATA_PIN_NUM) 
            if(data16[i] & __mask16(k)) 
                GPIOSetMask |= (dev->dataPinMask)[k];
            else 
                GPIOClearMask |= (dev->dataPinMask)[k];
        
                /// Export data to output
        GPIO.out_w1ts = GPIOSetMask;
        GPIO.out_w1tc = GPIOClearMask;

        /// Make falling edge of WRITE pin
        __p16SetLowWritePin(dev);
        __p16DelayHalfPeriod();
        /// Make raising edge of WRITE pin
        __p16SetHighWritePin(dev);
        __p16DelayHalfPeriod();
    }

    __exitCriticalSection(&(dev->mutex));
    __p16ReturnWithLog(OKE, "p16WriteArray() : OKE");
    returnERR_NULL:
        __exitCriticalSection(&(dev->mutex));
        __p16ReturnWithLog(ERR_NULL, "p16WriteArray() : ERR_NULL");
}

def p16ReadArray(parallel16Dev_t * dev, p16Data_t * data16, p16Size_t dataSize){
    __p16Entry("p16ReadArray(%p, %p, %d)", dev, data16, dataSize);

    __enterCriticalSection(&(dev->mutex));

    /// Read each byte
    REP(i, 0, dataSize){
        /// Make falling edge of READ pin
        __p16SetLowReadPin(dev);
        __p16DelayHalfPeriod();
        /// Make raising edge of READ pin
        __p16SetHighReadPin(dev);
        __p16DelayHalfPeriod();
        /// Snapshot input register once
        uint32_t currentGPIO = GPIO.in;
        /// Read data from 16 data pins
        data16[i] =
            ((currentGPIO & dev->dataPinMask[0]) ? 0x1 : 0) | 
            ((currentGPIO & dev->dataPinMask[1]) ? 0x2 : 0) | 
            ((currentGPIO & dev->dataPinMask[2]) ? 0x4 : 0) | 
            ((currentGPIO & dev->dataPinMask[3]) ? 0x8 : 0) | 
            ((currentGPIO & dev->dataPinMask[4]) ? 0x10 : 0) | 
            ((currentGPIO & dev->dataPinMask[5]) ? 0x20 : 0) | 
            ((currentGPIO & dev->dataPinMask[6]) ? 0x40 : 0) | 
            ((currentGPIO & dev->dataPinMask[7]) ? 0x80 : 0) | 
            ((currentGPIO & dev->dataPinMask[8]) ? 0x100 : 0) | 
            ((currentGPIO & dev->dataPinMask[9]) ? 0x200 : 0) | 
            ((currentGPIO & dev->dataPinMask[10]) ? 0x400 : 0) | 
            ((currentGPIO & dev->dataPinMask[11]) ? 0x800 : 0) | 
            ((currentGPIO & dev->dataPinMask[12]) ? 0x1000 : 0) | 
            ((currentGPIO & dev->dataPinMask[13]) ? 0x2000 : 0) | 
            ((currentGPIO & dev->dataPinMask[14]) ? 0x4000 : 0) | 
            ((currentGPIO & dev->dataPinMask[15]) ? 0x8000 : 0);
    }

    __exitCriticalSection(&(dev->mutex));
    __p16ReturnWithLog(OKE, "p16ReadArray() : OKE");
    returnERR_NULL:
        __exitCriticalSection(&(dev->mutex));
        __p16ReturnWithLog(ERR_NULL, "p16ReadArray() : ERR_NULL");
}