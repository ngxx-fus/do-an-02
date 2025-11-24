/// @file   oled128x64Helpers.h
/// @brief  Helper macros for OLED 128x64 display; Includes coordinate boundary checking and related utilities.

#ifndef __OLED128x64_HELPERS_H__
#define __OLED128x64_HELPERS_H__

/// @brief Check if (row, col) coordinate is within OLED boundaries
/// @param row Pixel row (0–63)
/// @param col Pixel column (0–127)
#define __xy_check(row, col, min_r, max_r, min_c, max_c) \
    (((row) >= (min_r)) && ((row) <= (max_r)) && \
     ((col) >= (min_c)) && ((col) <= (max_c)))

#endif