#ifndef __SYSTEM_STAGE_H__
#define __SYSTEM_STAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef PRINT_HEADER_COMPILE_MESSAGE
#pragma message ("AppConfig/SystemStage.h")
#endif

/* --- State Definitions (Magic Numbers) --- */

/* Initialization Stages (0xFFA0 -> 0xFFAF) */
/* Range: 16 stages (0x0 -> 0xF) */
#ifndef SYSTEM_INIT_0
    /// @brief System Init Phase 0 (Base Base)
    #define SYSTEM_INIT_0           0xFFA0
#endif

#ifndef SYSTEM_INIT_F
    /// @brief System Init Phase F (Final Init Stage)
    #define SYSTEM_INIT_F           0xFFAF
#endif

/* Operational Stages */
#ifndef SYSTEM_RUNNING
    /// @brief System is running normally (Magic: 0xFFB0)
    #define SYSTEM_RUNNING          0xFFB0
#endif

#ifndef SYSTEM_STOPPED
    /// @brief System has stopped/halted (Magic: 0xFFC0)
    #define SYSTEM_STOPPED          0xFFC0
#endif

/* --- Global Variable --- */

/// @brief Global system state variable.
/// @note  Must be defined in main.c (volatile ensures memory visibility).
extern volatile uint32_t SYSTEM_STAGE;

/* --- Check Macros --- */

/// @brief Calculate the magic number for a specific Init Phase N.
#define SYSTEM_INIT_N(N)            (SYSTEM_INIT_0 | ((N) & 0xF))

/// @brief Check if system is specifically in Init Phase N (0 to 15/0xF)
/// @param N The stage index (0 to 15)
#define IS_SYSTEM_INIT_N(N)         (SYSTEM_STAGE == SYSTEM_INIT_N(N))

/// @brief Check if system is ANY initialization phase (0x0 to 0xF)
/// @note  Checks if the high nibbles match 0xFFA-
#define IS_SYSTEM_INITIALIZING()    ((SYSTEM_STAGE & 0xFFF0) == SYSTEM_INIT_0)

/// @brief Check if system is running
#define IS_SYSTEM_RUNNING()         (SYSTEM_STAGE == SYSTEM_RUNNING)

/// @brief Check if system is stopped
#define IS_SYSTEM_STOPPED()         (SYSTEM_STAGE == SYSTEM_STOPPED)

/* --- Set Macros --- */

/// @brief Set specific Init stage N (0 to 15)
#define SET_SYSTEM_INIT_N(N)        (SYSTEM_STAGE = (SYSTEM_INIT_0 | ((N) & 0xF)))

/// @brief Set specific Init stage 0
#define SET_SYSTEM_INIT_0()         (SYSTEM_STAGE = SYSTEM_INIT_0)

/// @brief Set System to Running
#define SET_SYSTEM_RUNNING()        (SYSTEM_STAGE = SYSTEM_RUNNING)

/// @brief Set System to Stopped
#define SET_SYSTEM_STOPPED()        (SYSTEM_STAGE = SYSTEM_STOPPED)

/* --- Transition Macros --- */

/// @brief Move to the next logical stage automatically.
/// @details INIT_0 -> ... -> INIT_F -> RUNNING.
#define MOVE_NEXT_STAGE()           do { \
                                        if (SYSTEM_STAGE >= SYSTEM_INIT_0 && SYSTEM_STAGE < SYSTEM_INIT_F) { \
                                            SYSTEM_STAGE++; \
                                        } else if (SYSTEM_STAGE == SYSTEM_INIT_F) { \
                                            SYSTEM_STAGE = SYSTEM_RUNNING; \
                                        } \
                                    } while(0)

/* --- Wait Macros --- */

/// @brief Block execution until System Stage switches from ANY INIT to RUNNING.
/// @param ... Action to execute while waiting (e.g., vTaskDelay(10) or watchdog feed).
#define INIT_DO_WAIT(...)           do { \
                                        while (IS_SYSTEM_INITIALIZING()) { \
                                            __VA_ARGS__; \
                                        } \
                                    } while(0)

#ifdef __cplusplus
}
#endif

#endif /// __SYSTEM_STAGE_H__