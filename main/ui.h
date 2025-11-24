/// @file   monitor.h
/// @brief  Monitor & Display Logic; Handles LCD initialization, rendering loop, and visualization demos (Oscilloscope/Charts).

#include "../helper/general.h"
#include "../comDriver/lcd32/lcd32.h"
#include "../cds/circularBuffer/cbuff.h"

/// @brief Structure defining grid layout properties for graphs.
typedef struct gridInfo_t{
    dim_t cellSize; ///< Size of a single cell in pixels.
    dim_t colNum;   ///< Number of columns.
    dim_t rowNum;   ///< Number of rows.
    dim_t rowStep;  ///< Step size (pixels) between rows.
    dim_t colStep;  ///< Step size (pixels) between columns.
} gridInfo_t;

/// @brief Lookup table for different grid density configurations.
static const gridInfo_t gridInfoTable[] = {
    {  1, 320, 240, 1, 1 },     ///< Config 0: 1x1 step
    {  2, 160, 120, 2, 2 },     ///< Config 1: 2x2 step
    {  4,  80,  60, 4, 4 },     ///< Config 2: 4x4 step
    {  5,  64,  48, 5, 5 },     ///< Config 3: 5x5 step
    {  8,  40,  30, 8, 8 },     ///< Config 4: 8x8 step
    { 10,  32,  24,10,10 },     ///< Config 5: 10x10 step
    { 16,  20,  15,16,16 },     ///< Config 6: 16x16 step
    { 20,  16,  12,20,20 },     ///< Config 7: 20x20 step
    { 40,   8,   6,40,40 },     ///< Config 8: 40x40 step
    { 80,   4,   3,80,80 }      ///< Config 9: 80x80 step
};

/// @brief Color definitions (RGB565 format).
#define COLOR_ERROR             0xc8a6  ///< Color for error messages.
#define COLOR_GIRD              0xef9d  ///< Color for grid lines.
#define COLOR_AXIS              0x2104  ///< Color for axis lines.
#define COLOR_AXIS_TEXT         0xFF    ///< Color for axis text labels.
#define COLOR_LEGEND_BG         0xef7d  ///< Background color for legends.
#define COLOR_CHART_01          0xd6a0  ///< Color for chart series 1.
#define COLOR_CHART_02          0xf96e  ///< Color for chart series 2.
#define COLOR_CHART_03          0x356c  ///< Color for chart series 3.
#define COLOR_CHART_04          0x449e  ///< Color for chart series 4.

/// @brief Pointer to the global LCD device instance.
lcd32Dev_t * lcd;

/// @brief Global flag for screen state management.
volatile flag_t screenFlag = SYSTEM_INIT;

/// @brief Mutex for protecting screenFlag access.
portMUX_TYPE screenFlagMutex = portMUX_INITIALIZER_UNLOCKED;

/// @brief Bit definitions for screen control flags.
enum SCREEN_FLAG_BIT_ORDER {
    SCREEN_RENDER = 0,      ///< Flag bit to request screen rendering (Flush).
    SCREEN_TURN_ON = 0,     ///< Flag bit to turn on the screen.
    SCREEN_TURN_OFF = 0,    ///< Flag bit to turn off the screen.
    SCREEN_FLAG_BIT_ORDER_NUMBER,
};

/// @brief Initialize the LCD device and configure pins.
void lcdInit(){
    __entry("lcdInit()");
    lcd32CreateDevice(&lcd);
    lcd32DataPin_t dataPin = {
        .__0 = LCD32_DB0,   .__1 = LCD32_DB1,   .__2 = LCD32_DB2,   .__3 = LCD32_DB3,
        .__4 = LCD32_DB4,   .__5 = LCD32_DB5,   .__6 = LCD32_DB6,   .__7 = LCD32_DB7,
        .__8 = LCD32_DB8,   .__9 = LCD32_DB9,   .__10 = LCD32_DB10, .__11 = LCD32_DB11,
        .__12 = LCD32_DB12, .__13 = LCD32_DB13, .__14 = LCD32_DB14, .__15 = LCD32_DB15
    };
    lcd32ControlPin_t ctlPin = {
        .r = LCD32_RD,  .w = LCD32_WR,
        .rs= LCD32_RS,  .cs= LCD32_CS,
        .rst=LCD32_RST, .bl= LCD32_BL
    };
    lcd32ConfigDevice(lcd, &dataPin, &ctlPin, LCD32_MAX_ROW, LCD32_MAX_COL);
    lcd32StartUpDevice(lcd);
    // lcd32FillCanvas(lcd, 15);
    // lcd32FlushCanvas(lcd);

    __exit("lcdInit()");
}

/// @brief Display the project introduction screen.
void lcdShowIntroScreen(){
    __entry("lcdShowIntroScreen()");

    lcd32FillCanvas(lcd, 0xFFFF); // Black background

    lcd32DrawText(lcd, 40, 5, "DO AN 02", &fontHeading01, 0x0152);
    lcd32DrawText(lcd, 80, 5, "[1] Nguyen Thanh Phu - 22119211", &fontHeading03, 0x2925);
    lcd32DrawText(lcd, 100, 5, "[2] Nguyen Van Quoc  - 22119222", &fontHeading03, 0x2925);

    lcd32FlushCanvas(lcd);

    vTaskDelay(pdMS_TO_TICKS(1000)); // Display for 2 seconds

    __exit("lcdShowIntroScreen()");
}

/// @cond
/// HELPER FUNCTIONS (Internal Drawing Demos) /////////////////////////////////////////////////////

void lcdDrawCaro(uint8_t type) {
    // 1. Fill the canvas background with white
    lcd32FillCanvas(lcd, 0xFFFF); 

    // Define the color for the grid lines (black)
    color_t gridColor = COLOR_GIRD; 

    // 2. Get grid info from lookup table
    gridInfo_t grid = gridInfoTable[type];

    // 3. Draw horizontal lines
    for (dim_t i = 1; i < grid.rowNum; i++) {
        dim_t y = i * grid.rowStep;
        lcd32DrawLine(lcd, y, 0, y, LCD32_MAX_COL - 1, gridColor);
    }

    // 4. Draw vertical lines
    for (dim_t j = 1; j < grid.colNum; j++) {
        dim_t x = j * grid.colStep;
        lcd32DrawLine(lcd, 0, x, LCD32_MAX_ROW - 1, x, gridColor);
    }

    // Optional: signal redraw
    // __setFlagBit(screenFlag, SCREEN_RENDER);
}

void oscDemo(){
    lcdDrawCaro(7);
    // lcd32DrawLine(lcd, 120-1, 0, 120-1, 320-1, COLOR_AXIS);
    lcd32DrawLine(lcd, 120+0, 0, 120+0, 320-1, COLOR_AXIS);
    // lcd32DrawLine(lcd, 120+1, 0, 120+1, 320-1, COLOR_AXIS);
    // lcd32DrawLine(lcd, 0, 20-1, 240-1, 20-1, COLOR_AXIS);
    lcd32DrawLine(lcd, 0, 20, 240-1, 20, COLOR_AXIS);
    // lcd32DrawLine(lcd, 0, 20+1, 240-1, 20+1, COLOR_AXIS);
    char buf[32];
    REP(i, 0, 15){
        sprintf(buf, "+%02d", i);
        lcd32DrawText(lcd, 120+8, 13 + (20*(i)), buf, &fontNote, COLOR_AXIS_TEXT);
    }

    REP(i, 1, 6){
        sprintf(buf, "+%02d", 6-i);
        lcd32DrawText(lcd, 2 + (20*(i)), 5, buf, &fontNote, COLOR_AXIS_TEXT);
    }
    sprintf(buf, "%02d", 0);
    lcd32DrawText(lcd, 2 + 20*6, 5, buf, &fontNote, COLOR_AXIS_TEXT);
    REP(i, 7, 12){
        sprintf(buf, "-%02d", i-6);
        lcd32DrawText(lcd, 2 + (20*(i)), 5, buf, &fontNote, COLOR_AXIS_TEXT);
    }

    lcdPoint_t point[12];

    point[0].col = 20 + genRandNum(esp_timer_get_time())%10;  point[0].row = genRandNum(esp_timer_get_time())%240;
    point[1].col = 31 + genRandNum(esp_timer_get_time())%10;  point[1].row = genRandNum(esp_timer_get_time())%240;
    point[2].col = 47 + genRandNum(esp_timer_get_time())%10;  point[2].row = genRandNum(esp_timer_get_time())%240;
    point[3].col = 68 + genRandNum(esp_timer_get_time())%10;  point[3].row = genRandNum(esp_timer_get_time())%240;
    point[4].col = 82 + genRandNum(esp_timer_get_time())%10;  point[4].row = genRandNum(esp_timer_get_time())%240;
    point[5].col = 101 + genRandNum(esp_timer_get_time())%10; point[5].row = genRandNum(esp_timer_get_time())%240;
    point[6].col = 128 + genRandNum(esp_timer_get_time())%10; point[6].row = genRandNum(esp_timer_get_time())%240;
    point[7].col = 149 + genRandNum(esp_timer_get_time())%10; point[7].row = genRandNum(esp_timer_get_time())%240;
    point[8].col = 177 + genRandNum(esp_timer_get_time())%10; point[8].row = genRandNum(esp_timer_get_time())%240;
    point[9].col = 193 + genRandNum(esp_timer_get_time())%10; point[9].row = genRandNum(esp_timer_get_time())%240;
    point[10].col = 218 + genRandNum(esp_timer_get_time())%10; point[10].row = genRandNum(esp_timer_get_time())%240;
    point[11].col = 236 + genRandNum(esp_timer_get_time())%83; point[11].row = genRandNum(esp_timer_get_time())%240;
    lcd32DrawPolygon(lcd, point, 12, COLOR_CHART_01);

    point[0].col = 20 + genRandNum(esp_timer_get_time())%10;  point[0].row = genRandNum(esp_timer_get_time()) % 240;
    point[1].col = 48 + genRandNum(esp_timer_get_time())%10;  point[1].row = genRandNum(esp_timer_get_time()) % 240;
    point[2].col = 56 + genRandNum(esp_timer_get_time())%10;  point[2].row = genRandNum(esp_timer_get_time()) % 240;
    point[3].col = 69 + genRandNum(esp_timer_get_time())%10;  point[3].row = genRandNum(esp_timer_get_time()) % 240;
    point[4].col = 87 + genRandNum(esp_timer_get_time())%10;  point[4].row = genRandNum(esp_timer_get_time()) % 240;
    point[5].col = 109 + genRandNum(esp_timer_get_time())%10;  point[5].row = genRandNum(esp_timer_get_time()) % 240;
    point[6].col = 123 + genRandNum(esp_timer_get_time())%10; point[6].row = genRandNum(esp_timer_get_time()) % 240;
    point[7].col = 146 + genRandNum(esp_timer_get_time())%20; point[7].row = genRandNum(esp_timer_get_time()) % 240;
    point[8].col = 165 + genRandNum(esp_timer_get_time())%20; point[8].row = genRandNum(esp_timer_get_time()) % 240;
    point[9].col = 189 + genRandNum(esp_timer_get_time())%20; point[9].row = genRandNum(esp_timer_get_time()) % 240;
    point[10].col = 214 + genRandNum(esp_timer_get_time())%20; point[10].row = genRandNum(esp_timer_get_time()) % 240;
    point[11].col = 236 + genRandNum(esp_timer_get_time())%83; point[11].row = genRandNum(esp_timer_get_time()) % 240;
    lcd32DrawLineChart(lcd, point, 12, COLOR_CHART_02);

    point[0].col = 20 + genRandNum(esp_timer_get_time())%10;  point[0].row = genRandNum(esp_timer_get_time()) % 240;
    point[1].col = 48 + genRandNum(esp_timer_get_time())%10;  point[1].row = genRandNum(esp_timer_get_time()) % 240;
    point[2].col = 56 + genRandNum(esp_timer_get_time())%10;  point[2].row = genRandNum(esp_timer_get_time()) % 240;
    point[3].col = 69 + genRandNum(esp_timer_get_time())%10;  point[3].row = genRandNum(esp_timer_get_time()) % 240;
    point[4].col = 87 + genRandNum(esp_timer_get_time())%10;  point[4].row = genRandNum(esp_timer_get_time()) % 240;
    point[5].col = 109 + genRandNum(esp_timer_get_time())%10;  point[5].row = genRandNum(esp_timer_get_time()) % 240;
    point[6].col = 123 + genRandNum(esp_timer_get_time())%10; point[6].row = genRandNum(esp_timer_get_time()) % 240;
    point[7].col = 146 + genRandNum(esp_timer_get_time())%20; point[7].row = genRandNum(esp_timer_get_time()) % 240;
    point[8].col = 165 + genRandNum(esp_timer_get_time())%20; point[8].row = genRandNum(esp_timer_get_time()) % 240;
    point[9].col = 189 + genRandNum(esp_timer_get_time())%20; point[9].row = genRandNum(esp_timer_get_time()) % 240;
    point[10].col = 214 + genRandNum(esp_timer_get_time())%20; point[10].row = genRandNum(esp_timer_get_time()) % 240;
    point[11].col = 236 + genRandNum(esp_timer_get_time())%83; point[11].row = genRandNum(esp_timer_get_time()) % 240;
    lcd32DrawLineChart(lcd, point, 12, COLOR_CHART_03);

    point[0].col = 20 + genRandNum(esp_timer_get_time())%10;  point[0].row = genRandNum(esp_timer_get_time())%240;
    point[1].col = 31 + genRandNum(esp_timer_get_time())%10;  point[1].row = genRandNum(esp_timer_get_time())%240;
    point[2].col = 47 + genRandNum(esp_timer_get_time())%10;  point[2].row = genRandNum(esp_timer_get_time())%240;
    point[3].col = 68 + genRandNum(esp_timer_get_time())%10;  point[3].row = genRandNum(esp_timer_get_time())%240;
    point[4].col = 82 + genRandNum(esp_timer_get_time())%10;  point[4].row = genRandNum(esp_timer_get_time())%240;
    point[5].col = 101 + genRandNum(esp_timer_get_time())%10; point[5].row = genRandNum(esp_timer_get_time())%240;
    point[6].col = 128 + genRandNum(esp_timer_get_time())%10; point[6].row = genRandNum(esp_timer_get_time())%240;
    point[7].col = 149 + genRandNum(esp_timer_get_time())%10; point[7].row = genRandNum(esp_timer_get_time())%240;
    point[8].col = 177 + genRandNum(esp_timer_get_time())%10; point[8].row = genRandNum(esp_timer_get_time())%240;
    point[9].col = 193 + genRandNum(esp_timer_get_time())%10; point[9].row = genRandNum(esp_timer_get_time())%240;
    point[10].col = 218 + genRandNum(esp_timer_get_time())%10; point[10].row = genRandNum(esp_timer_get_time())%240;
    point[11].col = 236 + genRandNum(esp_timer_get_time())%83; point[11].row = genRandNum(esp_timer_get_time())%240;
    lcd32DrawLineChart(lcd, point, 12, COLOR_CHART_04);

    point[0].row = 1; point[0].col = 200;
    point[1].row = 25; point[1].col = 200;
    point[2].row = 25; point[2].col = 318;
    point[3].row = 1; point[3].col = 318;
    point[4].row = 1; point[4].col = 200;

    lcd32DrawFilledPolygon(lcd, point, 5, COLOR_LEGEND_BG);

    lcd32DrawLine(lcd, 8, 205, 8, 205+20, COLOR_CHART_01);
    lcd32DrawLine(lcd, 16, 205, 16, 205+20, COLOR_CHART_02);
    lcd32DrawText(lcd, 8+2, 205+20+5, "Chart 01", &fontNote, COLOR_CHART_01);
    lcd32DrawText(lcd, 16+2, 205+20+5, "Chart 02", &fontNote, COLOR_CHART_02);

    lcd32DrawLine(lcd, 8, 259, 8, 259+20, COLOR_CHART_03);
    lcd32DrawLine(lcd, 16, 259, 16, 259+20, COLOR_CHART_04);
    lcd32DrawText(lcd, 8+2, 259+20+5, "Chart 03", &fontNote, COLOR_CHART_03);
    lcd32DrawText(lcd, 16+2, 259+20+5, "Chart 04", &fontNote, COLOR_CHART_04);
}

void spiDemo(){
    lcdDrawCaro(7);
    // lcd32DrawLine(lcd, 120-1, 0, 120-1, 320-1, COLOR_AXIS);
    lcd32DrawLine(lcd, 120+0, 0, 120+0, 320-1, COLOR_AXIS);
    // lcd32DrawLine(lcd, 120+1, 0, 120+1, 320-1, COLOR_AXIS);
    // lcd32DrawLine(lcd, 0, 20-1, 240-1, 20-1, COLOR_AXIS);
    lcd32DrawLine(lcd, 0, 20, 240-1, 20, COLOR_AXIS);
    // lcd32DrawLine(lcd, 0, 20+1, 240-1, 20+1, COLOR_AXIS);
    char buf[32];
    REP(i, 0, 15){
        sprintf(buf, "+%02d", i);
        lcd32DrawText(lcd, 120+8, 13 + (20*(i)), buf, &fontNote, COLOR_AXIS_TEXT);
    }
    REP(i, 1, 6){
        sprintf(buf, "+%02d", 6-i);
        lcd32DrawText(lcd, 2 + (20*(i)), 5, buf, &fontNote, COLOR_AXIS_TEXT);
    }
    sprintf(buf, "%02d", 0);
    lcd32DrawText(lcd, 2 + 20*6, 5, buf, &fontNote, COLOR_AXIS_TEXT);
    REP(i, 7, 12){
        sprintf(buf, "-%02d", i-6);
        lcd32DrawText(lcd, 2 + (20*(i)), 5, buf, &fontNote, COLOR_AXIS_TEXT);
    }
    /////////////////////////////////////////////////
    char buff[64];

    // sprintf(buff, "This mode %lu is not available now! (%ld)");
    /*
    sprintf(buff, " (%ld)");
    */
    lcd32DrawText(lcd, 50, 5, buff, &fontHeading03,COLOR_ERROR);

}

/// @endcond

/// @brief Task to handle screen updates, rendering, and power states based on event flags.
/// @param pv Task parameter (unused).
void screenControlTask(void * pv){
    WAIT_SYSTEM_INIT_COMPLETED();

    __entry("screenControlTask()");
    
    flag_t localScreenFlag;
    while(!IS_SYSTEM_STOPPED){
        PERFORM_IN_CRITICAL(&screenFlagMutex,
            localScreenFlag = screenFlag;
            screenFlag = 0;
        );

        while (__isnot_zero(localScreenFlag)){
            if(__hasFlagBitSet(localScreenFlag, SCREEN_RENDER)){
                __clearFlagBit(localScreenFlag, SCREEN_RENDER);
                lcd32FlushCanvas(lcd);
            }
            if(__hasFlagBitSet(localScreenFlag, SCREEN_TURN_ON)){
                __clearFlagBit(localScreenFlag, SCREEN_TURN_ON);
                lcd32WakeFromSleep(lcd);
            }
            if(__hasFlagBitSet(localScreenFlag, SCREEN_TURN_OFF)){
                __clearFlagBit(localScreenFlag, SCREEN_TURN_OFF);
                lcd32PutToSleep(lcd);
            }
            if(__isnot_zero(localScreenFlag)){
                __sys_err("[screenControlTask] Warning: Unrecognized screenFlag bits: 0x%08x", localScreenFlag);
                localScreenFlag = 0;
            }
        }

        vTaskDelay(0);
    }

    __exit("screenControlTask()");
}