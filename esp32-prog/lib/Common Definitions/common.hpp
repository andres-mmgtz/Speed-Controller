#ifndef common_hpp
#define common_hpp

#include "stdint.h"

typedef struct
{
    uint32_t pulse_count; // 1600 pulses per revolution, each channel registers 800 pulses per revolution (rising and falling edge)
    int8_t direction;     // 1 for CW, -1 for CCW
} encoder_data_t;

typedef struct
{
    int16_t setpoint_rpm;
    int16_t current_rpm;
    int16_t error;
    int16_t control_signal;
} gui_data_t;

#endif