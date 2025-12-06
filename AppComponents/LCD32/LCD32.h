/**
 * @file LCD32.h
 * @brief Driver definitions for 320x240 LCD (16-bit Parallel Interface)
 * @details Extends P16Dev_t driver with specific LCD control pins (Backlight).
 * @author Nguyen Thanh Phu
 */

#ifndef __LCD32_H__
#define __LCD32_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../AppConfig/All.h"
#include "../../AppUtils/All.h"
#include "../../AppESPWrap/All.h"

#include "../P16Com/P16Com.h"

// For GFX fonts support
#include "esp_rom_sys.h" // For rom delay
#include "rom/ets_sys.h"

// Define rudimentary GFX structures if not available elsewhere
#ifndef _GFX_FONT_DEFINED
#define _GFX_FONT_DEFINED
typedef struct { 
    uint16_t bitmapOffset; 
    uint8_t  width; 
    uint8_t  height; 
    uint8_t  xAdvance; 
    int8_t   xOffset; 
    int8_t   yOffset; 
} GFXglyph;

typedef struct { 
    uint8_t  *bitmap; 
    GFXglyph *glyph; 
    uint8_t  first; 
    uint8_t  last; 
    uint8_t  yAdvance; 
} GFXfont;
#endif

/// @brief Point structure for polygon drawing
typedef struct {
    int16_t col; // x
    int16_t row; // y
} LCDPoint_t;

#define LCD32_LOG_SECTION
#define LCD32_UTILS_SECTION

#define LCD32_THREAD_SAFE_EN        1

/// @brief Use PSRAM to store the canvas (buffer)
#define LCD32_CANVAS_IN_PSRAM_EN    1

/// @brief Enable standard logging for this module
#define LCD32_LOG_EN                1
/// @brief Enable verbose/detailed logging
#define LCD32_LOG1_EN               1
/// @brief Enable error logging
#define LCD32_ERR_EN                1
/// @brief Enable function entry tracing
#define LCD32_LOG_ENTRY             1
/// @brief Enable function exit tracing
#define LCD32_LOG_EXIT              1

/// @brief  Dimension type for LCD
typedef int16_t                     Dim_t;
/// @brief  16-bit color type for LCD
typedef uint16_t                    Color_t;

#define LCD32_DEFAULT_H             240 
#define LCD32_DEFAULT_W             320 
#define LCD32_DEFAULT_ORIENTATION   0 
#define LCD32_DEFAULT_COLOR_BIT     sizeof(Color_t)

/// Predefined colors
#include "LCD32Colors.h"
/// Datasheet commands
#include "LCD32Cmds.h"

/// @brief Status flags for the driver
enum LCD320x240PositiveStatusFlag_e {
    __P16COM_RESERVED               = 0x00000001, ///< Reserved for P16Com compatibility
    LCD32_INITIALIZED               = 0x00000002, ///< Driver is initialized and ready
};

/// @brief LCD Device Structure
/// @details Implements memory overlay to extend P16Dev_t without breaking compatibility.
typedef struct LCD32Dev_t{
    union {
        /// @brief Base P16Com object (Allows casting to P16Dev_t*)
        P16Dev_t P16Com;

        /// @brief Overlay struct to access specific LCD pins directly
        struct {
            /// @brief Skips P16Com Control pins (Read, Write, CS, RS, Reset)
            Pin_t       _pad_ctl_arr[P16COM_CTL_PIN_NUM];
            
            /// @brief Backlight Control Pin (Overlays on P16Com Reserved0)
            Pin_t       BrightLight;
            
            /// @brief Skips remaining P16Com Control padding (Reserved1, Reserved2)
            Pin_t       _pad_ctl_end[P16COM_CTL_PIN_NUM_W_PADDING - P16COM_CTL_PIN_NUM - 1];
            
            /// @brief Skips P16Com Data pins (DatPinArr)
            Pin_t       _pad_dat_arr[P16COM_DAT_PIN_NUM_W_PADDING];
            
            /// @brief Status Flags (Must align with P16Com StatusFlag)
            uint32_t    StatusFlag;

            /// @brief Skips P16Com CtlIOMask (uint64_t)
            uint64_t    _pad_CtlMask;

            /// @brief Skips P16Com DatIOMask (uint64_t)
            uint64_t    _pad_DatMask;
        };
    };
    
    Dim_t Width;        ///< Current display width
    Dim_t Height;       ///< Current display height
    Dim_t Orientation;  ///< Current orientation (0-3)
    Color_t *Canvas;    ///< Frame buffer pointer (if used)
    #if (LCD32_THREAD_SAFE_EN == 1)
        SemaphoreHandle_t mutex; ///< Mutex for thread safety
    #endif
} LCD32Dev_t;

/* --- FUNCTION PROTOTYPES --- */

/// @brief Allocates memory for a new LCD32Dev_t object and Canvas
/// @return Pointer to the new object or NULL if malloc fails
LCD32Dev_t *        LCD32New();

/// @brief Deallocate memory for LCD32Dev_t object and Canvas
/// @param Dev (LCD32Dev_t *) Pointer to the LCD32Dev_t object to be deleted
void                LCD32Delete(LCD32Dev_t * Dev);

/// @brief Configure pin mapping for LCD
/// @param Dev (LCD32Dev_t *) Pointer to the device object
/// @param CtlPins (const Pin_t *) Array of 6 pins: [RD, WR, CS, RS, RST, BL]
/// @param DatPins (const Pin_t *) Array of 16 pins: [D0..D15]
/// @return STAT_OKE or Error Code
DefaultRet_t        LCD32Config(LCD32Dev_t * Dev, const Pin_t * CtlPins, const Pin_t * DatPins);

/// @brief Initialize the LCD (GPIOs, Reset, Startup Sequence)
/// @param Dev (LCD32Dev_t *) Pointer to the LCD32Dev_t object
/// @return STAT_OKE on success
DefaultRet_t        LCD32Init(LCD32Dev_t * Dev);

/// @brief Re-run configuration logic
/// @param Dev (LCD32Dev_t *) Pointer to the LCD32Dev_t object
/// @return STAT_OKE on success
DefaultRet_t        LCD32ReConfig(LCD32Dev_t * Dev);

/// @brief Set the active drawing area window on the LCD
/// @param Dev (LCD32Dev_t *) Pointer to the device object
/// @param x (Dim_t) The starting column (x-coordinate)
/// @param y (Dim_t) The starting row (y-coordinate)
/// @param w (Dim_t) The width of the window
/// @param h (Dim_t) The height of the window
void                LCD32SetAddressWindow(LCD32Dev_t * Dev, Dim_t x, Dim_t y, Dim_t w, Dim_t h);

/// @brief Flush the internal Canvas buffer to the display
/// @param Dev (LCD32Dev_t *) Pointer to the device object
void                LCD32FlushCanvas(LCD32Dev_t *Dev);

/// @brief Fill the entire canvas with a single color
/// @param Dev (LCD32Dev_t *) Pointer to the device object
/// @param Color (Color_t) The color to fill the canvas with
void                LCD32FillCanvas(LCD32Dev_t *Dev, Color_t Color);

/// @brief Draw a single pixel on the canvas (with bounds check)
/// @param Dev (LCD32Dev_t *) Pointer to the device object
/// @param Row (Dim_t) The row (y-coordinate) of the pixel
/// @param Col (Dim_t) The column (x-coordinate) of the pixel
/// @param Color (Color_t) The color of the pixel
void                LCD32SetCanvasPixel(LCD32Dev_t *Dev, Dim_t Row, Dim_t Col, Color_t Color);

/// @brief Write a single pixel directly to the LCD (bypassing canvas)
/// @param Dev (LCD32Dev_t *) Pointer to the device object
/// @param Row (Dim_t) The row (y-coordinate) of the pixel
/// @param Col (Dim_t) The column (x-coordinate) of the pixel
/// @param Color (Color_t) The color of the pixel
void                LCD32DirectlyWritePixel(LCD32Dev_t *Dev, Dim_t Row, Dim_t Col, Color_t Color);

/* --- DRAWING PRIMITIVES --- */

/// @brief Draw a line using Bresenham's algorithm
/// @param Dev (LCD32Dev_t *) Pointer to the device object
/// @param r0 (Dim_t) The starting row (y-coordinate)
/// @param c0 (Dim_t) The starting column (x-coordinate)
/// @param r1 (Dim_t) The ending row (y-coordinate)
/// @param c1 (Dim_t) The ending column (x-coordinate)
/// @param Color (Color_t) The color of the line
DefaultRet_t        LCD32DrawLine(LCD32Dev_t *Dev, Dim_t r0, Dim_t c0, Dim_t r1, Dim_t c1, Color_t Color);

/// @brief Draw a thick line
/// @param Dev (LCD32Dev_t *) Pointer to the device object
/// @param r0 (Dim_t) The starting row (y-coordinate)
/// @param c0 (Dim_t) The starting column (x-coordinate)
/// @param r1 (Dim_t) The ending row (y-coordinate)
/// @param c1 (Dim_t) The ending column (x-coordinate)
/// @param Color (Color_t) The color of the line
/// @param Thickness (Dim_t) The thickness of the line in pixels
DefaultRet_t        LCD32DrawThickLine(LCD32Dev_t *Dev, Dim_t r0, Dim_t c0, Dim_t r1, Dim_t c1, Color_t Color, Dim_t Thickness);

/// @brief Draw an empty rectangle
/// @param Dev (LCD32Dev_t *) Pointer to the device object
/// @param rTopLeft (Dim_t) The top row of the rectangle
/// @param cTopLeft (Dim_t) The left column of the rectangle
/// @param rBottomRight (Dim_t) The bottom row of the rectangle
/// @param cBottomRight (Dim_t) The right column of the rectangle
/// @param EdgeSize (Dim_t) The thickness of the border
/// @param Color (Color_t) The color of the border
DefaultRet_t        LCD32DrawEmptyRect(LCD32Dev_t *Dev, Dim_t rTopLeft, Dim_t cTopLeft, Dim_t rBottomRight, Dim_t cBottomRight, Dim_t EdgeSize, Color_t Color);

/// @brief Draw a polygon outline
/// @param Dev (LCD32Dev_t *) Pointer to the device object
/// @param Points (const LCDPoint_t *) Pointer to an array of points (vertices)
/// @param N (size_t) The number of points in the array
/// @param Color (Color_t) The color of the polygon outline
DefaultRet_t        LCD32DrawPolygon(LCD32Dev_t *Dev, const LCDPoint_t *Points, size_t N, Color_t Color);

/// @brief Draw a filled polygon (Scanline algorithm)
/// @param Dev (LCD32Dev_t *) Pointer to the device object
/// @param Points (const LCDPoint_t *) Pointer to an array of points (vertices)
/// @param N (size_t) The number of points in the array
/// @param Color (Color_t) The color to fill the polygon with
DefaultRet_t        LCD32DrawFilledPolygon(LCD32Dev_t *Dev, const LCDPoint_t *Points, size_t N, Color_t Color);

/// @brief Draw a character using GFX font
/// @param Dev (LCD32Dev_t *) Pointer to the device object
/// @param r (Dim_t) The top row coordinate for the character
/// @param c (Dim_t) The left column coordinate for the character
/// @param Ch (char) The character to draw
/// @param Font (const GFXfont *) Pointer to the GFX font
/// @param Color (Color_t) The color of the character
DefaultRet_t        LCD32DrawChar(LCD32Dev_t *Dev, Dim_t r, Dim_t c, char Ch, const GFXfont *Font, Color_t Color);

/// @brief Draw a string using GFX font
/// @param Dev (LCD32Dev_t *) Pointer to the device object
/// @param r (Dim_t) The top row coordinate for the string
/// @param c (Dim_t) The left column coordinate for the string
/// @param Str (const char *) The null-terminated string to draw
/// @param Font (const GFXfont *) Pointer to the GFX font
/// @param Color (Color_t) The color of the string
DefaultRet_t        LCD32DrawText(LCD32Dev_t *Dev, Dim_t r, Dim_t c, const char *Str, const GFXfont *Font, Color_t Color);

/* --- LOW LEVEL HELPERS --- */

/// @brief Write a command byte to the LCD
/// @param Dev (LCD32Dev_t *) Pointer to the device object
/// @param Cmd (uint16_t) The command to write
void                LCD32WriteCmd(LCD32Dev_t * Dev, uint16_t Cmd);

/// @brief Write a data byte/word to the LCD
/// @param Dev (LCD32Dev_t *) Pointer to the device object
/// @param Data (uint16_t) The data to write
void                LCD32WriteData(LCD32Dev_t * Dev, uint16_t Data);

/* --- MACROS & LOGGING --- */

#ifdef LCD32_LOG_SECTION

    #if (LCD32_LOG_EN == 1)
        /// @brief Log standard info message
        #define LCD32Log(...)                 SysLog(__VA_ARGS__)
    #else 
        #define LCD32Log(...)
    #endif 

    #if (LCD32_ERR_EN == 1)
        /// @brief Log error message
        #define LCD32Err(...)                 SysErr(__VA_ARGS__)
    #else
        #define LCD32Err(...)
    #endif 

    #if (LCD32_LOG_EXIT == 1)
        /// @brief Log function exit
        #define LCD32Exit(...)                SysExit(__VA_ARGS__)
        
        /// @brief Log function exit and return a value
        #define LCD32ReturnWithLog(ret, ...)  SysExit(__VA_ARGS__); return ret;
    #else
        #define LCD32Exit(...)
        #define LCD32ReturnWithLog(ret, ...)  return ret;
    #endif 

    #if (LCD32_LOG_ENTRY == 1)
        /// @brief Log function entry
        #define LCD32Entry(...)               SysEntry(__VA_ARGS__)
    #else 
        #define LCD32Entry(...)
    #endif 

    #if (LCD32_LOG1_EN == 1)
        /// @brief Log verbose/detailed message
        #define LCD32Log1(...)                SysLogVer(__VA_ARGS__)
    #else 
        #define LCD32Log1(...)
    #endif 

#endif /// LCD32_LOG_SECTION

#ifdef LCD32_UTILS_SECTION

    /// @brief Turn OFF Backlight (Active Low logic assumed for P-Channel/PNP, check HW!)
    #define LCD32SetHighBrightLightPin(lcdDev)      IOSet(1ULL << (lcdDev)->BrightLight)

    /// @brief Turn ON Backlight
    #define LCD32SetLowBrightLightPin(lcdDev)       IOClr(1ULL << (lcdDev)->BrightLight)

    /// @brief Macros for Transaction Management
    #define LCD32SetDataTransaction(dev)            P16SetHighRegSelPin((&(dev->P16Com)))
    #define LCD32SetCommandTransaction(dev)         P16SetLowRegSelPin((&(dev->P16Com)))
    #define LCD32StartTransaction(dev)              P16SetLowChipSelPin((&(dev->P16Com)))
    #define LCD32StopTransaction(dev)               P16SetHighChipSelPin((&(dev->P16Com)))

#endif /// LCD32_UTILS_SECTION

#ifdef __cplusplus
}
#endif

#endif /// __LCD32_H__