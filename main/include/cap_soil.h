#pragma once

typedef enum {
  CAP_SOIL_1 = 1, // GPIO 32 (ADC1_CH4)
  CAP_SOIL_2,     // GPIO 33 (ADC1_CH5)
} cap_soil_number_t;

void cap_soil_init(void);
int cap_soil_read_raw(cap_soil_number_t number);
void cap_soil_delete(void);
