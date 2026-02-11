#include "encoder.hpp"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_intr_alloc.h"
#include "freertos/semphr.h"
#include "common.hpp"

extern HardwareSerial Serial;

QueueHandle_t encoder_intr_queue = NULL;
QueueHandle_t encoder_count_queue = NULL;

void encoder_init()
{
    encoder_intr_queue = xQueueCreate(1, sizeof(uint8_t));
    encoder_count_queue = xQueueCreate(1, sizeof(encoder_data_t));

    BaseType_t encoder_task_init = xTaskCreatePinnedToCore(
        encoder_task,   // Task function
        "ENCODER Task", // Name of the task (for debugging purposes)
        4096,           // Stack size (in bytes)
        NULL,           // Task parameters (not used in this case)
        1,              // Task priority (adjust as needed)
        NULL,           // Task handle (not used in this case)
        0               // Core ID (0 or 1 for ESP32)
    );

    gpio_config_t encoder_gpio_config = {
        .pin_bit_mask = GPIO_INTR_INPUT,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE};

    gpio_config(&encoder_gpio_config);
    gpio_install_isr_service(ESP_INTR_FLAG_EDGE);
    gpio_isr_handler_add(ENCODER_PIN_A, gpio_isr_handler, (void *)ENCODER_PIN_A);
    gpio_isr_handler_add(ENCODER_PIN_B, gpio_isr_handler, (void *)ENCODER_PIN_B);

    if (encoder_task_init != pdPASS || encoder_intr_queue == NULL || encoder_count_queue == NULL)
    {
        Serial.println("ERROR: Encoder initialization failed - queue creation failed");
        // esp_restart();
    }
}

// ISR Handler
void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(encoder_intr_queue, &gpio_num, NULL);
}

void encoder_task(void *pvParameters)
{
    uint32_t gpio_num;
    uint8_t state_A;
    uint8_t state_B;
    uint8_t current_state = 0;
    uint8_t previous_state = 0;

    int8_t direction_lookup[4][4] = {
        {0, 1, -1, 0},
        {-1, 0, 0, 1},
        {1, 0, 0, -1},
        {0, -1, 1, 0}};

    uint32_t pulse_count = 0;
    int8_t direction = 0;

    encoder_data_t encoder_data = {
        .pulse_count = 0,
        .direction = 0};

    for (;;)
    {
        if (xQueueReceive(encoder_intr_queue, &gpio_num, portMAX_DELAY) == pdTRUE)
        {
            state_A = gpio_get_level(ENCODER_PIN_A);
            state_B = gpio_get_level(ENCODER_PIN_B);

            current_state = (state_A << 1) | state_B;

            direction = direction_lookup[previous_state][current_state];
            pulse_count++;

            encoder_data.pulse_count = pulse_count;
            // Disabled due to connection problems, placed a constant
            // encoder_data.direction = direction;
            encoder_data.direction = 1;

            // Serial.printf("C: %d, L: %d, DIR: %d\n", current_state, previous_state, direction);

            xQueueOverwrite(encoder_count_queue, &encoder_data);

            previous_state = current_state;
        }
    }
}