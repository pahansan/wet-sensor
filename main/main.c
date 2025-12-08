#include "cap_soil.h"
#include "rgb_led.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

int cut(int raw) {
  if (raw >= 2700)
    return 1700;
  if (raw < 980) {
    return -1;
  }
  if (raw <= 1000)
    return 0;
  return raw - 1000;
}

double raw_to_voltage(int raw) { return (double)raw / 4095.0 * 3.3; }
double raw_to_percent(int raw) { return (double)(1700 - cut(raw)) / 17.0; }
int raw_to_green(int raw) { return (int)((double)(1700 - cut(raw)) * 0.15); }
int raw_to_red(int raw) { return (int)((double)cut(raw) * 0.15); }

void raw_to_led(int raw, led_number_t number) {
  int cutted = cut(raw);
  if (cutted == -1) {
    rgb_led_set(0, 0, 255, number);
    return;
  }
  int red_value = (int)((double)cutted * 0.15);
  rgb_led_set(red_value, 255 - red_value, 0, number);
}

void app_main(void) {
  cap_soil_init();
  rgb_led_init();

  while (1) {
    int raw = cap_soil_read_raw(CAP_SOIL_1);
    printf("Capacitive soil %d Raw Data: %d, v: %f, %%: %f | ", CAP_SOIL_1, raw,
           raw_to_voltage(raw), raw_to_percent(raw));
    raw_to_led(raw, LED_1);
    raw = cap_soil_read_raw(CAP_SOIL_2);
    printf("Capacitive soil %d Raw Data: %d, v: %f, %%: %f\n", CAP_SOIL_2, raw,
           raw_to_voltage(raw), raw_to_percent(raw));
    raw_to_led(raw, LED_2);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
