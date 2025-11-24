<<<<<<< HEAD
#ifndef __OLED128X64_CMDS_H__
#define __OLED128X64_CMDS_H__

/*=========================================================================
    SSD1306 OLED 128x64 COMMANDS (Datasheet Reference)
    --------------------------------------------------
    Each command configures internal registers of SSD1306.
    Use control byte = 0x00 before sending a command.
=========================================================================*/

/* Fundamental Commands */
#define SSD1306_SET_CONTRAST                0x81  // Set contrast control (followed by 1 byte: 0x00–0xFF)
#define SSD1306_ENTIRE_DISPLAY_RESUME       0xA4  // Resume to RAM content display
#define SSD1306_ENTIRE_DISPLAY_ON           0xA5  // Entire display ON (ignore RAM, all pixels ON)
#define SSD1306_NORMAL_DISPLAY              0xA6  // Set normal display (0=OFF, 1=ON)
#define SSD1306_INVERT_DISPLAY              0xA7  // Inverse display (0=ON, 1=OFF)
#define SSD1306_DISPLAY_OFF                 0xAE  // Set display OFF (sleep)
#define SSD1306_DISPLAY_ON                  0xAF  // Set display ON

/* Scrolling Commands */
#define SSD1306_DEACTIVATE_SCROLL           0x2E  // Stop scrolling
#define SSD1306_ACTIVATE_SCROLL             0x2F  // Start scrolling
#define SSD1306_RIGHT_HORIZONTAL_SCROLL     0x26  // Setup right horizontal scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL      0x27  // Setup left horizontal scroll
#define SSD1306_VERTICAL_AND_RIGHT_SCROLL   0x29  // Setup vertical & right horizontal scroll
#define SSD1306_VERTICAL_AND_LEFT_SCROLL    0x2A  // Setup vertical & left horizontal scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA    0xA3  // Set vertical scroll area (2 bytes: top, area)

/* Addressing Setting Commands */
#define SSD1306_SET_MEMORY_MODE             0x20  // Set memory addressing mode (1 byte: 0x00=Horizontal, 0x01=Vertical, 0x02=Page)
#define SSD1306_SET_COLUMN_ADDR             0x21  // Set column address (2 bytes: start, end)
#define SSD1306_SET_PAGE_ADDR               0x22  // Set page address (2 bytes: start, end)

/* Hardware Configuration Commands */
#define SSD1306_SET_START_LINE(line)        (0x40 | ((line) & 0x3F)) // Set display start line (0–63)
#define SSD1306_SET_SEG_REMAP_0             0xA0  // Column address 0 → SEG0
#define SSD1306_SET_SEG_REMAP_127           0xA1  // Column address 127 → SEG0 (mirrored)
#define SSD1306_SET_MULTIPLEX_RATIO         0xA8  // Set multiplex ratio (1 byte: 0x0F–0x3F)
#define SSD1306_SET_COM_SCAN_INC            0xC0  // Scan from COM0 to COM[N-1]
#define SSD1306_SET_COM_SCAN_DEC            0xC8  // Scan from COM[N-1] to COM0 (remap)
#define SSD1306_SET_DISPLAY_OFFSET          0xD3  // Set display offset (1 byte: vertical shift)
#define SSD1306_SET_COM_PINS                0xDA  // Set COM pins hardware configuration (1 byte)

/* Timing & Driving Scheme Setting Commands */
#define SSD1306_SET_DISPLAY_CLOCK_DIV       0xD5  // Set display clock divide ratio/oscillator frequency (1 byte)
#define SSD1306_SET_PRECHARGE               0xD9  // Set pre-charge period (1 byte)
#define SSD1306_SET_VCOM_DETECT             0xDB  // Set VCOMH deselect level (1 byte)

/* Charge Pump Command */
#define SSD1306_CHARGE_PUMP                 0x8D  // Enable/disable charge pump (1 byte: 0x14=enable, 0x10=disable)

/* Control */
#define SSD1306_COMMAND                     0x0
#define SSD1306_DATA                        0x40

=======
/// @file   oled128x64Cmds.h
/// @brief  SSD1306 OLED 128x64 command definitions.
/// @details Provides macros for display control, addressing, scrolling, and hardware configuration.
///          Each command configures internal registers of SSD1306.
///          Use control byte = 0x00 before sending a command.

#ifndef __OLED128X64_CMDS_H__
#define __OLED128X64_CMDS_H__

#include "stdint.h"

/** @name Fundamental Commands
 * Basic display configuration (Contrast, On/Off, Invert).
 * @{
 */

/// @brief Set contrast control (followed by 1 byte: 0x00–0xFF)
#define SSD1306_SET_CONTRAST                0x81

/// @brief Resume to RAM content display
#define SSD1306_ENTIRE_DISPLAY_RESUME       0xA4

/// @brief Entire display ON (ignore RAM, all pixels ON)
#define SSD1306_ENTIRE_DISPLAY_ON           0xA5

/// @brief Set normal display (0=OFF, 1=ON)
#define SSD1306_NORMAL_DISPLAY              0xA6

/// @brief Inverse display (0=ON, 1=OFF)
#define SSD1306_INVERT_DISPLAY              0xA7

/// @brief Set display OFF (sleep)
#define SSD1306_DISPLAY_OFF                 0xAE

/// @brief Set display ON
#define SSD1306_DISPLAY_ON                  0xAF

/** @} */

/** @name Scrolling Commands
 * Hardware scrolling configuration.
 * @{
 */

/// @brief Stop scrolling
#define SSD1306_DEACTIVATE_SCROLL           0x2E

/// @brief Start scrolling
#define SSD1306_ACTIVATE_SCROLL             0x2F

/// @brief Setup right horizontal scroll
#define SSD1306_RIGHT_HORIZONTAL_SCROLL     0x26

/// @brief Setup left horizontal scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL      0x27

/// @brief Setup vertical & right horizontal scroll
#define SSD1306_VERTICAL_AND_RIGHT_SCROLL   0x29

/// @brief Setup vertical & left horizontal scroll
#define SSD1306_VERTICAL_AND_LEFT_SCROLL    0x2A

/// @brief Set vertical scroll area (2 bytes: top, area)
#define SSD1306_SET_VERTICAL_SCROLL_AREA    0xA3

/** @} */

/** @name Addressing Setting Commands
 * Memory addressing modes and pointer positioning.
 * @{
 */

/// @brief Set memory addressing mode (1 byte: 0x00=Horizontal, 0x01=Vertical, 0x02=Page)
#define SSD1306_SET_MEMORY_MODE             0x20

/// @brief Set column address (2 bytes: start, end)
#define SSD1306_SET_COLUMN_ADDR             0x21

/// @brief Set page address (2 bytes: start, end)
#define SSD1306_SET_PAGE_ADDR               0x22

/** @} */

/** @name Hardware Configuration Commands
 * Panel hardware mapping configuration.
 * @{
 */

/// @brief Set display start line (0–63)
#define SSD1306_SET_START_LINE(line)        (0x40 | ((line) & 0x3F))

/// @brief Column address 0 mapped to SEG0
#define SSD1306_SET_SEG_REMAP_0             0xA0

/// @brief Column address 127 mapped to SEG0 (mirrored)
#define SSD1306_SET_SEG_REMAP_127           0xA1

/// @brief Set multiplex ratio (1 byte: 0x0F–0x3F)
#define SSD1306_SET_MULTIPLEX_RATIO         0xA8

/// @brief Scan from COM0 to COM[N-1]
#define SSD1306_SET_COM_SCAN_INC            0xC0

/// @brief Scan from COM[N-1] to COM0 (remap)
#define SSD1306_SET_COM_SCAN_DEC            0xC8

/// @brief Set display offset (1 byte: vertical shift)
#define SSD1306_SET_DISPLAY_OFFSET          0xD3

/// @brief Set COM pins hardware configuration (1 byte)
#define SSD1306_SET_COM_PINS                0xDA

/** @} */

/** @name Timing & Driving Scheme
 * Oscillator frequency and pre-charge periods.
 * @{
 */

/// @brief Set display clock divide ratio/oscillator frequency (1 byte)
#define SSD1306_SET_DISPLAY_CLOCK_DIV       0xD5

/// @brief Set pre-charge period (1 byte)
#define SSD1306_SET_PRECHARGE               0xD9

/// @brief Set VCOMH deselect level (1 byte)
#define SSD1306_SET_VCOM_DETECT             0xDB

/** @} */

/** @name Charge Pump
 * Internal DC/DC voltage generator control.
 * @{
 */

/// @brief Enable/disable charge pump (1 byte: 0x14=enable, 0x10=disable)
#define SSD1306_CHARGE_PUMP                 0x8D

/** @} */

/** @name Control Bytes
 * Co/D/C bits for I2C data stream.
 * @{
 */

/// @brief Continuation bit=0, D/C=0 (Command)
#define SSD1306_COMMAND                     0x00

/// @brief Continuation bit=0, D/C=1 (Data)
#define SSD1306_DATA                        0x40

/** @} */

>>>>>>> master
#endif /* __OLED128X64_CMDS_H__ */