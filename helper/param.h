/// @file   param.h
/// @brief  Defines logic level types and constants; includes `level_t` typedef and `BIN_LEVEL` enum (LOW/HIGH).

#ifndef __PARAM_H__
#define __PARAM_H__

#include "stdio.h"
#include "stdint.h"

/// Type of logic level
typedef uint8_t     level_t; 

enum BIN_LEVEL{
    LOW = 0,
    HIGH = 1
};

#endif 