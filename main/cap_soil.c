#include "include/cap_soil.h"

#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"
#include <sys/_intsup.h>

#define CAP_SOIL_ADC_UNIT ADC_UNIT_1
#define CAP_SOIL_ADC_CHANNEL_1 ADC_CHANNEL_4
#define CAP_SOIL_ADC_CHANNEL_2 ADC_CHANNEL_5

#define ADC_ATTENUATION ADC_ATTEN_DB_12

adc_oneshot_unit_handle_t adc_handle;

void cap_soil_init(void) {
  adc_oneshot_unit_init_cfg_t init_config = {
      .unit_id = CAP_SOIL_ADC_UNIT,
      .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
  };

  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

  adc_oneshot_chan_cfg_t chan_cfg = {
      .bitwidth = ADC_BITWIDTH_DEFAULT,
      .atten = ADC_ATTEN_DB_12,
  };

  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, CAP_SOIL_ADC_CHANNEL_1,
                                             &chan_cfg));

  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, CAP_SOIL_ADC_CHANNEL_2,
                                             &chan_cfg));
}

int cap_soil_read_raw(cap_soil_number_t number) {
  int adc_raw;
  switch (number) {
  case CAP_SOIL_1:
    ESP_ERROR_CHECK(
        adc_oneshot_read(adc_handle, CAP_SOIL_ADC_CHANNEL_1, &adc_raw));
    break;
  case CAP_SOIL_2:
    ESP_ERROR_CHECK(
        adc_oneshot_read(adc_handle, CAP_SOIL_ADC_CHANNEL_2, &adc_raw));
    break;
  }
  return adc_raw;
}

double cap_soil_read_voltage(cap_soil_number_t number) {
  int adc_raw;
  switch (number) {
  case CAP_SOIL_1:
    ESP_ERROR_CHECK(
        adc_oneshot_read(adc_handle, CAP_SOIL_ADC_CHANNEL_1, &adc_raw));
    break;
  case CAP_SOIL_2:
    ESP_ERROR_CHECK(
        adc_oneshot_read(adc_handle, CAP_SOIL_ADC_CHANNEL_2, &adc_raw));
    break;
  }
  return (double)adc_raw / 4095.0 * 3.3;
}

void cap_soil_delete() { ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle)); }
