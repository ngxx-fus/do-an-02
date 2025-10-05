#ifndef __OLED128X64_LOG_H__
#define __OLED128X64_LOG_H__

/*
 *  Set level of log, there are the description of the leves:
 *  Noted that: the greater value of log includes all lower log types!:
 *      1 : Only log error
 *      2 : Log entry (+ params) and exit (+ return status)
 *      3 : Log state inside the function, ...
 *      4 : The lowest level of logs, it log all things, includes every byte will be sent!
*/
#define OLED_LOG_LEVEL 4

#if OLED_LOG_LEVEL >= 1
    #define __oledErr(...)       __err(  __VA_ARGS__ )
#else 
    #define __oledErr(...)
#endif
#if OLED_LOG_LEVEL >= 2
    #define __oledEntry(...)     __entry(  __VA_ARGS__ )
    #define __oledExit(...)      __exit(  __VA_ARGS__ )
#else 
    #define __oledEntry(...)
    #define __oledExit(...)
#endif 
#if OLED_LOG_LEVEL >= 3
    #define __oledLog(...)      __log(  __VA_ARGS__ )
#else 
    #define __oledLog(...)
#endif 
#if OLED_LOG_LEVEL >= 4
    #define __oledLog1(...)      __log(  __VA_ARGS__ )
#else 
    #define __oledLog1(...)
#endif 

#endif