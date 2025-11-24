<<<<<<< HEAD
=======
/// @file   lcd32Cmd.h
/// @brief  ILI9341 LCD command definitions; Provides all command codes and constants for controlling the 3.2" LCD.

>>>>>>> master
#ifndef __LCD32_CMD_H__
#define __LCD32_CMD_H__

#include "../../helper/general.h"

/*=========================================================================
    ILI9341 COMMANDS (Datasheet: ILI9341 V1.11)
    --------------------------------------------------
<<<<<<< HEAD
    Sources: ILI9341 Datasheet V1.11 [cite: 5]
=========================================================================*/

/*-------------------- Level 1 Commands [cite: 13] --------------------*/
// ... (Giữ nguyên các lệnh NOP đến READ_ID3) ...
=======
    Sources: 
        ILI9341 Datasheet V1.11 [cite: 5]
        https://www.waveshare.com/wiki/3.2inch_320x240_Touch_LCD_(D)
=========================================================================*/

/*-------------------- Level 1 Commands [cite: 13] --------------------*/
>>>>>>> master
#define ILI9341_NOP                        0x00  // No Operation [cite: 13]
#define ILI9341_SWRESET                    0x01  // Software Reset [cite: 13]
#define ILI9341_READ_ID                     0x04  // Read Display Identification Information [cite: 13]
#define ILI9341_READ_STATUS                 0x09  // Read Display Status [cite: 13]
#define ILI9341_READ_POWER_MODE             0x0A  // Read Display Power Mode [cite: 13]
#define ILI9341_READ_MADCTL                 0x0B  // Read Memory Access Control [cite: 13]
#define ILI9341_READ_PIXEL_FORMAT           0x0C  // Read Pixel Format [cite: 13]
#define ILI9341_READ_IMAGE_FORMAT           0x0D  // Read Image Format [cite: 13]
#define ILI9341_READ_SIGNAL_MODE            0x0E  // Read Signal Mode [cite: 13]
#define ILI9341_READ_SELF_DIAG              0x0F  // Read Self-Diagnostic Result [cite: 13]
#define ILI9341_ENTER_SLEEP                 0x10  // Enter Sleep Mode [cite: 13]
#define ILI9341_SLEEP_OUT                   0x11  // Sleep Out [cite: 13]
#define ILI9341_PARTIAL_MODE_ON             0x12  // Partial Mode ON [cite: 13]
#define ILI9341_NORMAL_DISPLAY_MODE_ON      0x13  // Normal Display Mode ON [cite: 13]
#define ILI9341_DISPLAY_INVERSION_OFF       0x20  // Display Inversion OFF [cite: 13]
#define ILI9341_DISPLAY_INVERSION_ON        0x21  // Display Inversion ON [cite: 13]
#define ILI9341_GAMMA_SET                   0x26  // Gamma Set (1 param) [cite: 13]
#define ILI9341_DISPLAY_OFF                 0x28  // Display OFF [cite: 13]
#define ILI9341_DISPLAY_ON                  0x29  // Display ON [cite: 13]
#define ILI9341_COLUMN_ADDRESS_SET          0x2A  // Column Address Set (4 params) [cite: 13]
#define ILI9341_PAGE_ADDRESS_SET            0x2B  // Page Address Set (4 params) [cite: 13]
#define ILI9341_MEMORY_WRITE                0x2C  // Memory Write (followed by data) [cite: 13]
#define ILI9341_COLOR_SET                   0x2D  // Color Set (LUT for 16->18 bit) (128 params) [cite: 13, 730]
#define ILI9341_MEMORY_READ                 0x2E  // Memory Read [cite: 13]
#define ILI9341_PARTIAL_AREA                0x30  // Partial Area (4 params) [cite: 13]
#define ILI9341_VERTICAL_SCROLL_DEFINITION  0x33  // Vertical Scrolling Definition (6 params) [cite: 13]
#define ILI9341_TEARING_LINE_OFF            0x34  // Tearing Effect Line OFF [cite: 13]
#define ILI9341_TEARING_LINE_ON             0x35  // Tearing Effect Line ON (1 param) [cite: 13]
#define ILI9341_MEMORY_ACCESS_CONTROL       0x36  // Memory Access Control (MADCTL) (1 param) [cite: 13]
#define ILI9341_VERTICAL_SCROLL_START       0x37  // Vertical Scrolling Start Address (2 params) [cite: 13]
#define ILI9341_IDLE_MODE_OFF               0x38  // Idle Mode OFF [cite: 13]
#define ILI9341_IDLE_MODE_ON                0x39  // Idle Mode ON [cite: 13]
#define ILI9341_PIXEL_FORMAT_SET            0x3A  // COLMOD: Pixel Format Set (1 param) [cite: 17]
#define ILI9341_WRITE_MEMORY_CONTINUE       0x3C  // Write Memory Continue [cite: 17]
#define ILI9341_READ_MEMORY_CONTINUE        0x3E  // Read Memory Continue [cite: 17]
#define ILI9341_SET_TEAR_SCANLINE           0x44  // Set Tear Scanline (2 params) [cite: 17]
#define ILI9341_GET_SCANLINE                0x45  // Get Scanline [cite: 17]
#define ILI9341_WRITE_DISPLAY_BRIGHTNESS    0x51  // Write Display Brightness (1 param) [cite: 17]
#define ILI9341_READ_DISPLAY_BRIGHTNESS     0x52  // Read Display Brightness [cite: 17]
#define ILI9341_WRITE_CTRL_DISPLAY          0x53  // Write CTRL Display (1 param) [cite: 17]
#define ILI9341_READ_CTRL_DISPLAY           0x54  // Read CTRL Display [cite: 17]
#define ILI9341_WRITE_CABC                  0x55  // Write Content Adaptive Brightness Control (1 param) [cite: 17]
#define ILI9341_READ_CABC                   0x56  // Read CABC [cite: 17]
#define ILI9341_WRITE_CABC_MIN_BRIGHT       0x5E  // Write CABC Minimum Brightness (1 param) [cite: 17]
#define ILI9341_READ_CABC_MIN_BRIGHT        0x5F  // Read CABC Minimum Brightness [cite: 17]
#define ILI9341_READ_ID1                    0xDA  // Read ID1 [cite: 17]
#define ILI9341_READ_ID2                    0xDB  // Read ID2 [cite: 17]
#define ILI9341_READ_ID3                    0xDC  // Read ID3 [cite: 17]

/*-------------------- Level 2 Commands [cite: 17] --------------------*/
#define ILI9341_RGB_INTERFACE_SIGNAL_CTRL   0xB0  // RGB Interface Signal Control (1 param) [cite: 17]
#define ILI9341_FRAME_RATE_NORMAL           0xB1  // Frame Rate Control (Normal) (2 params) [cite: 17]
#define ILI9341_FRAME_RATE_IDLE             0xB2  // Frame Rate Control (Idle) (2 params) [cite: 17]
#define ILI9341_FRAME_RATE_PARTIAL          0xB3  // Frame Rate Control (Partial) (2 params) [cite: 17]
#define ILI9341_DISPLAY_INVERSION_CTRL      0xB4  // Display Inversion Control (1 param) [cite: 17]
#define ILI9341_BLANKING_PORCH_CTRL         0xB5  // Blanking Porch Control (4 params) [cite: 17]
#define ILI9341_DISPLAY_FUNCTION_CTRL       0xB6  // Display Function Control (4 params) [cite: 17]
#define ILI9341_ENTRY_MODE_SET              0xB7  // Entry Mode Set (1 param) [cite: 17]
#define ILI9341_BACKLIGHT_CTRL_1            0xB8  // Backlight Control 1 (2 params) [cite: 17]
#define ILI9341_BACKLIGHT_CTRL_2            0xB9  // Backlight Control 2 (2 params) [cite: 17]
#define ILI9341_BACKLIGHT_CTRL_3            0xBA  // Backlight Control 3 (2 params) [cite: 17]
#define ILI9341_BACKLIGHT_CTRL_4            0xBB  // Backlight Control 4 (2 params) [cite: 17]
#define ILI9341_BACKLIGHT_CTRL_5            0xBC  // Backlight Control 5 (2 params) [cite: 17]
#define ILI9341_BACKLIGHT_CTRL_7            0xBE  // Backlight Control 7 (1 param) [cite: 17]
#define ILI9341_BACKLIGHT_CTRL_8            0xBF  // Backlight Control 8 (1 param) [cite: 17]
#define ILI9341_POWER_CONTROL_1             0xC0  // Power Control 1 (1 param, VRH) [cite: 17]
#define ILI9341_POWER_CONTROL_2             0xC1  // Power Control 2 (1 param, BT) [cite: 17]
#define ILI9341_VCOM_CONTROL_1              0xC5  // VCOM Control 1 (2 params, VMH, VML) [cite: 17]
#define ILI9341_VCOM_CONTROL_2              0xC7  // VCOM Control 2 (1 param, nVM, VMF) [cite: 17]
#define ILI9341_NV_MEMORY_WRITE             0xD0  // NV Memory Write (2 params) [cite: 17]
#define ILI9341_NV_MEMORY_PROTECT_KEY       0xD1  // NV Memory Protection Key (3 params) [cite: 17]
#define ILI9341_NV_MEMORY_STATUS_READ       0xD2  // NV Memory Status Read [cite: 17]
#define ILI9341_READ_ID4                    0xD3  // Read ID4 
#define ILI9341_POSITIVE_GAMMA_CORR         0xE0  // Positive Gamma Correction (15 params) 
#define ILI9341_NEGATIVE_GAMMA_CORR         0xE1  // Negative Gamma Correction (15 params) 
#define ILI9341_DIGITAL_GAMMA_CTRL1         0xE2  // Digital Gamma Control 1 (16 params) 
#define ILI9341_DIGITAL_GAMMA_CTRL2         0xE3  // Digital Gamma Control 2 (16 params) 
#define ILI9341_INTERFACE_CONTROL           0xF6  // Interface Control (3 params) 

/*-------------------- Extended Register Commands  --------------------*/
#define ILI9341_POWER_CONTROL_A             0xCB  // Power Control A (5 params) 
#define ILI9341_POWER_CONTROL_B             0xCF  // Power Control B (3 params) 
#define ILI9341_DRIVER_TIMING_CTRL_A_INT    0xE8  // Driver Timing Control A (Internal clock) (3 params) 
#define ILI9341_DRIVER_TIMING_CTRL_A_EXT    0xE9  // Driver Timing Control A (External clock) (3 params) 
#define ILI9341_DRIVER_TIMING_CTRL_B        0xEA  // Driver Timing Control B (2 params) 
#define ILI9341_POWER_ON_SEQ_CTRL           0xED  // Power On Sequence Control (4 params) 
#define ILI9341_ENABLE_3G                   0xF2  // Enable 3G (1 param) 
#define ILI9341_PUMP_RATIO_CONTROL          0xF7  // Pump Ratio Control (1 param) 

/*-------------------- Useful constants / data roles --------------------*/
#define ILI9341_DCX_COMMAND                0x00  // D/CX = 0 -> command
#define ILI9341_DCX_DATA                   0x01  // D/CX = 1 -> data

<<<<<<< HEAD
/*-------------------- Notes & usage tips --------------------*/
=======
>>>>>>> master

#endif /* __LCD32_CMD_H__ */