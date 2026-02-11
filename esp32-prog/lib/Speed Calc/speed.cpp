#include "speed.hpp"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "common.hpp"

extern HardwareSerial Serial;

extern encoder_data_t shared_encoder_data;
extern QueueHandle_t encoder_count_queue;

extern SemaphoreHandle_t gui_mutex;
extern gui_data_t shared_gui_data;

void speed_init()
{
    BaseType_t speed_task_init = xTaskCreatePinnedToCore(
        speed_task,               // Task function
        "Speed Calculation Task", // Name of the task (for debugging purposes)
        4096,                     // Stack size (in bytes)
        NULL,                     // Task parameters (not used in this case)
        1,                        // Task priority (adjust as needed)
        NULL,                     // Task handle (not used in this case)
        0                         // Core ID (0 or 1 for ESP32)
    );
}

void speed_task(void *pvParameters)
{
    TickType_t last_wake_time = xTaskGetTickCount();

    encoder_data_t encoder_data = {
        .pulse_count = 0,
        .direction = 0};

    int64_t last_time = esp_timer_get_time();
    int64_t current_time = 0;

    uint32_t current_pulse_count = 0;
    int8_t current_direction = 0;
    uint32_t last_pulse_count = 0;

    int16_t rpm_indicator = 0;

    for (;;)
    {
        xTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(5));

        // Get accurate time in microseconds between cycles
        current_time = esp_timer_get_time();

        if (xQueueReceive(encoder_count_queue, &encoder_data, pdMS_TO_TICKS(1)) == pdTRUE)
        {
            current_pulse_count = encoder_data.pulse_count;
            current_direction = encoder_data.direction;
        }
        rpm_indicator = (int16_t)((current_pulse_count - last_pulse_count) / ((current_time - last_time) / 1000000.0 * 1600) * 60 * current_direction);

        if (xSemaphoreTake(gui_mutex, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            shared_gui_data.current_rpm = rpm_indicator;
            xSemaphoreGive(gui_mutex);
        }
        // Serial.printf("RPM SERIAL: %d\n", rpm_indicator);
        // Serial.printf("RPM: %d, CP: %d, LP: %d, Dir: %d, T: %lld\n", rpm_indicator, current_pulse_count, last_pulse_count, current_direction, current_time - last_time);
        last_pulse_count = current_pulse_count;
        last_time = current_time;
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
