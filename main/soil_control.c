#include "include/soil_control.h"
#include "esp_log.h"
#include "include/cap_soil.h"
#include "include/rgb_led.h"
#include "nvs.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "SOIL";

static soil_settings_t settings = {.red_threshold = 20,
                                   .green_threshold = 80,
                                   .sensor1_enabled = true,
                                   .sensor2_enabled = true,
                                   .led1_enabled = true,
                                   .led2_enabled = true};

static int raw_to_percent(int raw) {
  int cut_raw;
  if (raw >= 2700) {
    cut_raw = 1700;
  } else if (raw < 980) {
    return 0;
  } else if (raw <= 1000) {
    cut_raw = 0;
  } else {
    cut_raw = raw - 1000;
  }

  double percent = (1700.0 - cut_raw) / 17.0;
  if (percent < 0)
    percent = 0;
  if (percent > 100)
    percent = 100;
  return (int)percent;
}

static void update_led(int sensor, int percent) {
  if (!settings.led1_enabled && sensor == 1) {
    rgb_led_set(0, 0, 0, LED_1);
    return;
  }
  if (!settings.led2_enabled && sensor == 2) {
    rgb_led_set(0, 0, 0, LED_2);
    return;
  }

  if (percent <= 0) {
    if (sensor == 1)
      rgb_led_set(255, 0, 0, LED_1);
    else
      rgb_led_set(255, 0, 0, LED_2);
  } else if (percent >= 100) {
    if (sensor == 1)
      rgb_led_set(0, 255, 0, LED_1);
    else
      rgb_led_set(0, 255, 0, LED_2);
  } else {
    int range = settings.green_threshold - settings.red_threshold;
    if (range <= 0)
      range = 1;
    int rel = (percent - settings.red_threshold);
    if (rel < 0)
      rel = 0;
    if (rel > range)
      rel = range;
    int ratio = (rel * 255) / range;

    uint8_t r = 255 - ratio;
    uint8_t g = ratio;
    if (sensor == 1)
      rgb_led_set(r, g, 0, LED_1);
    else
      rgb_led_set(r, g, 0, LED_2);
  }
}

void soil_control_init(void) {
  nvs_handle_t handle;
  esp_err_t err = nvs_open("soil", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "NVS open failed");
    return;
  }

  int32_t val;
  if (nvs_get_i32(handle, "red", &val) == ESP_OK)
    settings.red_threshold = (int)val;
  if (nvs_get_i32(handle, "green", &val) == ESP_OK)
    settings.green_threshold = (int)val;
  if (nvs_get_i32(handle, "s1", &val) == ESP_OK)
    settings.sensor1_enabled = (bool)val;
  if (nvs_get_i32(handle, "s2", &val) == ESP_OK)
    settings.sensor2_enabled = (bool)val;
  if (nvs_get_i32(handle, "l1", &val) == ESP_OK)
    settings.led1_enabled = (bool)val;
  if (nvs_get_i32(handle, "l2", &val) == ESP_OK)
    settings.led2_enabled = (bool)val;

  nvs_close(handle);

  if (settings.red_threshold < 0)
    settings.red_threshold = 0;
  if (settings.red_threshold > 100)
    settings.red_threshold = 100;
  if (settings.green_threshold < 0)
    settings.green_threshold = 0;
  if (settings.green_threshold > 100)
    settings.green_threshold = 100;
  if (settings.green_threshold < settings.red_threshold)
    settings.green_threshold = settings.red_threshold;

  ESP_LOGI(TAG, "Loaded: red=%d, green=%d, s1=%d, s2=%d, l1=%d, l2=%d",
           settings.red_threshold, settings.green_threshold,
           settings.sensor1_enabled, settings.sensor2_enabled,
           settings.led1_enabled, settings.led2_enabled);
}

void soil_control_save(void) {
  nvs_handle_t handle;
  esp_err_t err = nvs_open("soil", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "NVS open failed");
    return;
  }

  nvs_set_i32(handle, "red", settings.red_threshold);
  nvs_set_i32(handle, "green", settings.green_threshold);
  nvs_set_i32(handle, "s1", settings.sensor1_enabled);
  nvs_set_i32(handle, "s2", settings.sensor2_enabled);
  nvs_set_i32(handle, "l1", settings.led1_enabled);
  nvs_set_i32(handle, "l2", settings.led2_enabled);
  nvs_commit(handle);
  nvs_close(handle);
}

soil_settings_t soil_control_get_settings(void) { return settings; }

void soil_control_set_settings(const soil_settings_t *new_settings) {
  if (!new_settings)
    return;
  settings = *new_settings;
  if (settings.red_threshold < 0)
    settings.red_threshold = 0;
  if (settings.red_threshold > 100)
    settings.red_threshold = 100;
  if (settings.green_threshold < 0)
    settings.green_threshold = 0;
  if (settings.green_threshold > 100)
    settings.green_threshold = 100;
  if (settings.green_threshold < settings.red_threshold)
    settings.green_threshold = settings.red_threshold;
  soil_control_save();
}

int soil_control_get_moisture_percent(int sensor) {
  if (sensor != 1 && sensor != 2)
    return -1;
  bool enabled =
      (sensor == 1) ? settings.sensor1_enabled : settings.sensor2_enabled;
  if (!enabled)
    return -1;

  int raw = cap_soil_read_raw(sensor == 1 ? CAP_SOIL_1 : CAP_SOIL_2);
  int percent = raw_to_percent(raw);

  update_led(sensor, percent);

  return percent;
}

void soil_control_update_leds(void) {
  if (settings.sensor1_enabled) {
    int p1 = soil_control_get_moisture_percent(1);
    update_led(1, p1);
  } else {
    rgb_led_set(0, 0, 0, LED_1);
  }
  if (settings.sensor2_enabled) {
    int p2 = soil_control_get_moisture_percent(2);
    update_led(2, p2);
  } else {
    rgb_led_set(0, 0, 0, LED_2);
  }
}
