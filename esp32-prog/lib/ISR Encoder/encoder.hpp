#ifndef encoder_hpp
#define encoder_hpp

#include "stdint.h"

#define ENCODER_PIN_A GPIO_NUM_10
#define ENCODER_PIN_B GPIO_NUM_9
#define GPIO_INTR_INPUT ((1ULL << ENCODER_PIN_A) | (1ULL << ENCODER_PIN_B))
// #define GPIO_INTR_INPUT (1ULL << ENCODER_PIN_A)

void encoder_init();

void gpio_isr_handler(void *arg);

void encoder_task(void *pvParameters);

#endif