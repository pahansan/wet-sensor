#pragma once
#include <stdint.h>

typedef enum {
  LED_1 = 1, // GPIO 25 26 27
  LED_2      // GPIO 17 16 4
} led_number_t;

void rgb_led_init(void);
void rgb_led_set(uint8_t r, uint8_t g, uint8_t b, led_number_t number);