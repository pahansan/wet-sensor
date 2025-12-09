#pragma once
#include <stdbool.h>

typedef struct {
  int red_threshold;
  int green_threshold;
  bool sensor1_enabled;
  bool sensor2_enabled;
  bool led1_enabled;
  bool led2_enabled;
} soil_settings_t;

void soil_control_init(void);
void soil_control_save(void);
soil_settings_t soil_control_get_settings(void);
void soil_control_set_settings(const soil_settings_t *settings);
int soil_control_get_moisture_percent(int sensor);
void soil_control_update_leds(void);
