#pragma once
#include <stdint.h>

typedef enum {
  LED_1 = 1, // GPIO 25(R), 26(G), 27(B)
  LED_2      // GPIO 17(R), 16(G), 4(B)
} led_number_t;

void rgb_led_init(void);
void rgb_led_set(uint8_t r, uint8_t g, uint8_t b, led_number_t number);
