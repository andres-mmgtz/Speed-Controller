#include "gui.hpp"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "common.hpp"

extern HardwareSerial Serial;

SemaphoreHandle_t gui_mutex;
gui_data_t shared_gui_data = {
    .setpoint_rpm = 0,
    .current_rpm = 0,
    .error = 0,
    .control_signal = 0};

void gui_init()
{
    gui_mutex = xSemaphoreCreateMutex();

    BaseType_t gui_task_init = xTaskCreatePinnedToCore(
        gui_task,   // Task function
        "GUI Task", // Name of the task (for debugging purposes)
        4096,       // Stack size (in bytes)
        NULL,       // Task parameters (not used in this case)
        1,          // Task priority (adjust as needed)
        NULL,       // Task handle (not used in this case)
        1           // Core ID (0 or 1 for ESP32)
    );

    if (gui_task_init != pdPASS || gui_mutex == NULL)
    {
        // esp_restart();
    }
}

void gui_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    gui_data_t local_gui_data = {
        .setpoint_rpm = 0,
        .current_rpm = 0,
        .error = 0,
        .control_signal = 0};

    for (;;)
    {
        xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));

        if (xSemaphoreTake(gui_mutex, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            memcpy(&local_gui_data, &shared_gui_data, sizeof(gui_data_t));
            xSemaphoreGive(gui_mutex);
        }
        Serial.printf("RPM: %d\n", local_gui_data.current_rpm);
    }
}