#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Include necessary header files for project initialization.
#include "gui.hpp"
#include "encoder.hpp"
#include "speed.hpp"
// #include "controller.hpp"
// #include "power.hpp"

void setup()
{
  Serial.begin(115200);

  gui_init();
  encoder_init();
  speed_init();
  // controller_init();
  // power_init();
}

void loop()
{
  // Delete Arduino's default loop task.
  vTaskDelete(NULL);
}