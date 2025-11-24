/// @file   oled128x64.h
/// @brief  Header for OLED 128x64 driver; Provides device creation, configuration, pixel/canvas operations, and text rendering APIs.

#ifndef __OLED128X64_H__
#define __OLED128X64_H__

#include "../i2c/i2c.h"

#include "oled128x64Commands.h"
#include "oled128x64Log.h"
#include "oled128x64Helpers.h"
#include "oled128x64Definitions.h"

#include "../../fonts/fontUtils.h"

/*
      0 1 2  y    127
    0 ###############
    1 ###############
    x #######p#######
    7 ###############

    For 3D buffer:
        p(x, y) <-> frame[pgID][clID][rowID]
    For 1D buffer:
        + Byte access:      __byte_at(x, y): (integer(x/8)*127 + y)
        + Read operation:   __read_at(x, y):  (__byte_at(x, y) & (1<<(x%8)))
        + Write 1-bit operation: __write_1_at(x, y):  (__byte_at(x, y) |= (1<<(x%8)))
        + Write 0-bit operation: __write_0_at(x, y):  (__byte_at(x, y) &= (~(1<<(x%8))))
        + Write op: __write_at(x, y, bit): ((bit)?__write_1_at(x, y):__write_0_at(x, y))
*/

/// @brief Create and allocate a new OLED device descriptor.
/// @param oled Double pointer to the OLED device handle to be allocated.
/// @return OKE on success, or error code if allocation fails.
def createOLEDDevice(oled128x64Dev_t **oled);

/// @brief Configure the OLED device I²C pins and communication parameters.
/// @param oled Pointer to the OLED device descriptor.
/// @param scl  GPIO pin number used for the I²C SCL line.
/// @param sda  GPIO pin number used for the I²C SDA line.
/// @param config Configuration flags. See I2C_CONFIG_ORDER enum.
/// @return OKE on success, or error code if configuration fails.
def configOLEDDevice(oled128x64Dev_t *oled, pin_t scl, pin_t sda, flag_t config);

/// @brief Initialize and start up the OLED device (send initialization sequence).
/// @param oled Pointer to the OLED device descriptor.
/// @return OKE on success, or error code if initialization fails.
def startupOLEDDevice(oled128x64Dev_t *oled);

/// @brief Reset the OLED control registers to default state (top-left = (0,0)).
/// @param oled Pointer to the OLED device descriptor.
/// @return OKE on success.
def oledResetView(oled128x64Dev_t *oled);

/// @brief Turn on all pixels on the OLED display (override RAM content).
/// @param oled Pointer to the OLED device descriptor.
/// @return OKE on success.
def oledTurnOnAllPixels(oled128x64Dev_t *oled);

/// @brief Flush the current framebuffer (canvas) to the OLED display.
/// @param oled Pointer to the OLED device descriptor.
/// @return OKE on success.
def oledFlush(oled128x64Dev_t *oled);

/// @brief Display the content stored in OLED RAM (resume from “all pixels on” mode).
/// @param oled Pointer to the OLED device descriptor.
/// @return OKE on success.
def oledShowRAMContent(oled128x64Dev_t *oled);

/// @brief Fill the entire screen with a uniform color.
/// @param oled Pointer to the OLED device descriptor.
/// @param color Pixel value (1 = ON, 0 = OFF).
/// @return OKE on success.
def oledFillScreen(oled128x64Dev_t *oled, color_t color);

/// @brief Set a pixel in the OLED canvas to a given color (1 = ON, 0 = OFF).
/// @param oled Pointer to the OLED device descriptor.
/// @param row  Pixel row index (0–height-1).
/// @param col  Pixel column index (0–width-1).
/// @param c    Color value (1 = ON, 0 = OFF).
/// @return OKE on success, ERR_INVALID_ARG if coordinates are out of bounds.
def oledSetPixel(oled128x64Dev_t *oled, xy_t row, xy_t col, color_t c);

/// @brief Draw an empty (border-only) rectangle on the OLED canvas.
/// @param oled Pointer to the OLED device descriptor.
/// @param rTopLeft     Top-left corner row index.
/// @param cTopLeft     Top-left corner column index.
/// @param rBottomRight Bottom-right corner row index.
/// @param cBottomRight Bottom-right corner column index.
/// @param edgeSize     Thickness of rectangle border (in pixels).
/// @param color        Color value (1 = ON, 0 = OFF).
/// @return OKE on success, ERR_INVALID_ARG if coordinates are invalid.
def oledDrawEmptyRect(oled128x64Dev_t *oled, 
                      xy_t rTopLeft, xy_t cTopLeft,
                      xy_t rBottomRight, xy_t cBottomRight,
                      xy_t edgeSize, color_t color);

/// @brief Draw a text string on the OLED display using the active font.
/// @param oled Pointer to the OLED device descriptor.
/// @param r    Starting row (baseline position).
/// @param c    Starting column position.
/// @param str  Pointer to the null-terminated text string to render.
/// @param cl   Text color (1 = ON, 0 = OFF).
/// @param wrap Enable word wrapping (true = wrap at screen edge).
/// @param lineHeight Line spacing (in pixels); 0 to use font default.
/// @return OKE on success, ERR_INVALID_ARG if string is NULL or invalid.
def oledDrawText(oled128x64Dev_t *oled, xy_t r, xy_t c,
                 const char *str, color_t cl, bool wrap, byte_t lineHeight);

/// @brief Draw text with line param instead of row index of baseline
/// @param oled Pointer to the OLED device descriptor.
/// @param text Text to be draw
/// @param conf Config lineH, mode. 
/// @note  conf[7:0]:  lineIndex; Range: [0, (screenHeight/lineHeight)].
/// @note  conf[15:08]:  lineHeight;                Set 0 for auto determine.
/// @note  conf[16]: 0 - Call oledResetView(),      1 - Skip oledResetView()
/// @note  conf[17]: 0 - Call oledFillScreen(),     1 - Skip oledFillScreen()
/// @note  conf[18]: 0 - Call oledShowRAMContent(), 1 - Skip oledShowRAMContent()
/// @note  conf[19]: 0 - Call oledFlush(),          1 - Skip oledFlush()
void drawLineText(oled128x64Dev_t * oled, const char * text, flag_t conf);

#endif /* __OLED128X64_H__ */
