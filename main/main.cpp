#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "rom/gpio.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "lvgl.h"
#include "board.h"
#include "esp_timer.h"
#include "Arduino.h"
#include "ui/ui.h"
#include "main_arduino.cpp"
#include "aw9523.h"
#define TAG "MAIN"

extern void app_loop();
extern void app_setup();

static void increase_lvgl_tick(void* arg) {
    lv_tick_inc(portTICK_PERIOD_MS);
}

extern "C" void screen_init(void);

void setBoilerPrio(lv_event_t *e)
{

}
extern "C" void lvgl_task(void* arg) {
    screen_init();

    // Tick interface for LVGL
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = increase_lvgl_tick,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "periodic_gui",
        .skip_unhandled_events = false
    };
    esp_timer_handle_t periodic_timer;
    esp_timer_create(&periodic_timer_args, &periodic_timer);
    esp_timer_start_periodic(periodic_timer, portTICK_PERIOD_MS * 1000);

    ui_init();

    for (;;) {
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


extern "C" void app_main(void) {
    aw9523_softreset();
    aw9523_init(TOUCH_IIC_SDA, TOUCH_IIC_SDA);

    xTaskCreatePinnedToCore(lvgl_task, NULL, 8 * 1024, NULL, 5, NULL, 1);
    initArduino();
    app_setup();
    // never ending
    app_loop();
}
