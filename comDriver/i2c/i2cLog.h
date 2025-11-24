/// @file   i2cLog.h
/// @brief  Logging macros for I2C driver; Supports different log levels including error, entry/exit, function state, and detailed data logging.

#ifndef __I2C_LOG_H__
#define __I2C_LOG_H__

#include "../../helper/general.h"

/*
 *  Set level of log, there are the description of the leves:
 *  Noted that: the greater value of log includes all lower log types!:
 *      1 : Only log error
 *      2 : Log entry (+ params) and exit (+ return status)
 *      3 : Log state inside the function, ...
 *      4 : The lowest level of logs, it log all things, includes every byte will be sent!
*/
#define I2C_LOG_LEVEL 1

#if I2C_LOG_LEVEL >= 1
    #define __i2cErr(...)       __sys_err( __VA_ARGS__ )
#else 
    #define __i2cErr(...)
#endif
#if I2C_LOG_LEVEL >= 2
    #define __i2cEntry(...)     __entry( __VA_ARGS__ )
    #define __i2cExit(...)      __exit( __VA_ARGS__ )
#else 
    #define __i2cEntry(...)
    #define __i2cExit(...)
#endif 
#if I2C_LOG_LEVEL >= 3
    #define __i2cLog(...)      __sys_log( __VA_ARGS__ )
#else 
    #define __i2cLog(...)
#endif 
#if I2C_LOG_LEVEL >= 4
    #define __i2cLog1(...)      __sys_log( __VA_ARGS__ )
#else 
    #define __i2cLog1(...)
#endif 


#endif 