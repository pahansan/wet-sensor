#pragma once

typedef enum {
  CAP_SOIL_1 = 1, // GPIO 32
  CAP_SOIL_2,     // GPIO 33
} cap_soil_number_t;

void cap_soil_init();
int cap_soil_read_raw(cap_soil_number_t number);
double cap_soil_read_voltage(cap_soil_number_t number);
void cap_soil_delete();