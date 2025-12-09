#include "include/rgb_led.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define PIN_R_1 25
#define PIN_G_1 26
#define PIN_B_1 27

#define PIN_R_2 17
#define PIN_G_2 16
#define PIN_B_2 4

#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_RES LEDC_TIMER_8_BIT
#define LEDC_FREQ 5000

void rgb_led_set(uint8_t r, uint8_t g, uint8_t b, led_number_t number) {
  switch (number) {
  case LED_1:
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, r);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, g);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, b);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2);
    break;
  case LED_2:
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_3, r);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_3);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_4, g);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_4);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_5, b);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_5);
    break;
  }
}

void rgb_led_init(void) {
  ledc_timer_config_t timer = {
      .speed_mode = LEDC_MODE,
      .timer_num = LEDC_TIMER,
      .duty_resolution = LEDC_RES,
      .freq_hz = LEDC_FREQ,
      .clk_cfg = LEDC_AUTO_CLK,
  };
  ESP_ERROR_CHECK(ledc_timer_config(&timer));

  ledc_channel_config_t ch = {
      .speed_mode = LEDC_MODE,
      .timer_sel = LEDC_TIMER,
      .intr_type = LEDC_INTR_DISABLE,
      .duty = 0,
      .hpoint = 0,
      .flags.output_invert = 1,
  };

  ch.channel = LEDC_CHANNEL_0;
  ch.gpio_num = PIN_R_1;
  ESP_ERROR_CHECK(ledc_channel_config(&ch));
  ch.channel = LEDC_CHANNEL_1;
  ch.gpio_num = PIN_G_1;
  ESP_ERROR_CHECK(ledc_channel_config(&ch));
  ch.channel = LEDC_CHANNEL_2;
  ch.gpio_num = PIN_B_1;
  ESP_ERROR_CHECK(ledc_channel_config(&ch));
  ch.channel = LEDC_CHANNEL_3;
  ch.gpio_num = PIN_R_2;
  ESP_ERROR_CHECK(ledc_channel_config(&ch));
  ch.channel = LEDC_CHANNEL_4;
  ch.gpio_num = PIN_G_2;
  ESP_ERROR_CHECK(ledc_channel_config(&ch));
  ch.channel = LEDC_CHANNEL_5;
  ch.gpio_num = PIN_B_2;
  ESP_ERROR_CHECK(ledc_channel_config(&ch));

  rgb_led_set(0, 0, 0, LED_1);
  rgb_led_set(0, 0, 0, LED_2);
}
