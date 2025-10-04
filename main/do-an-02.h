#include "../helper/general.h"

// #include "../driver/"

void IRAM_ATTR buttonISR(void* pv){
    int64_t* testModeVariable = (int64_t*) pv;
    
    int64_t entryButtonISR =  esp_timer_get_time();
    if((GPIO.in & __mask32(BTN0)) == LOW){
        __log("Button is pressed!");
        __log("entryButtonISR = %ld", entryButtonISR);
        if(entryButtonISR - testModeVariable[1] < 100000){
            __log("testModeVariable[1] = %ld", testModeVariable[1]);
            __log("Button is pressed lesthan than 100000us, break test mode!");
            testModeVariable[0] = 0;
        }
        testModeVariable[1] = entryButtonISR;
    }else{
        __log("Button is released!");
    }
}

void ledTest(){
    /// Output
    gpio_config_t outPin = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = __mask32(PIN0) | __mask32(PIN1) |  
                        __mask32(PIN2) | __mask32(PIN3),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&outPin);

    /// Input + Int
    gpio_config_t inPin = {
        .intr_type      = GPIO_INTR_ANYEDGE,
        .mode           = GPIO_MODE_INPUT,
        .pin_bit_mask   = __mask32(BTN0),
        .pull_down_en   = GPIO_PULLDOWN_DISABLE,
        .pull_up_en     = GPIO_PULLUP_DISABLE
    };
    gpio_config(&inPin);

    /// Status variable 
    /// testModeVariable[0] : Control the test state, Set 0 to stop test mode
    /// testModeVariable[1] : Save last pressed
    int64_t testModeVariable[2] = {1, 0};

    /// Enable all interrupt
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(BTN0, buttonISR, (void*) testModeVariable);

    /// Running test mode
    while(testModeVariable[0] != 0){
        vTaskDelay(200/portTICK_PERIOD_MS);
        GPIO.out_w1ts = (__mask32(PIN0));
        vTaskDelay(200/portTICK_PERIOD_MS);
        GPIO.out_w1ts =  __mask32(PIN1);
        vTaskDelay(200/portTICK_PERIOD_MS);
        GPIO.out_w1ts = __mask32(PIN2);
        vTaskDelay(200/portTICK_PERIOD_MS);
        GPIO.out_w1ts = __mask32(PIN3);
        vTaskDelay(200/portTICK_PERIOD_MS);
        GPIO.out_w1tc = (__mask32(PIN0));
        vTaskDelay(200/portTICK_PERIOD_MS);
        GPIO.out_w1tc =  __mask32(PIN1);
        vTaskDelay(200/portTICK_PERIOD_MS);
        GPIO.out_w1tc = __mask32(PIN2);
        vTaskDelay(200/portTICK_PERIOD_MS);
        GPIO.out_w1tc = __mask32(PIN3);
    };

    gpio_uninstall_isr_service();
    gpio_reset_pin(PIN0);
    gpio_reset_pin(PIN1);
    gpio_reset_pin(PIN2);
    gpio_reset_pin(PIN3);

}