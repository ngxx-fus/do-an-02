#ifndef __PARALLEL16_DEF_H__
#define __PARALLEL16_DEF_H__

#define P16_COMMAND_MODE    0
#define P16_DATA_MODE       1

typedef uint16_t p16Data_t;

typedef struct p16DataPin_t{       /// Struct for data pin (only accepts GPIO number less than 32)
    /// 16-bit data pin
    pin_t       __0, __1, __2,  __3,  __4,  __5,  __6,  __7, 
                __8, __9, __10, __11, __12, __13, __14, __15;
} p16DataPin_t;

typedef struct p16ControlPin_t{   /// Struct for data pint
    pin_t       r;                      /// Read 
    pin_t       w;                      /// Write
    pin_t       cs;                     /// Chip select
    pin_t       rs;                     /// Register select
} p16ControlPin_t;

typedef struct parallel16Dev_t{
    p16DataPin_t        dataPin;
    p16ControlPin_t     controlPin;
    flag_t              dataPinMask;
    flag_t              controlPinMask;
} parallel16Dev_t;


#endif