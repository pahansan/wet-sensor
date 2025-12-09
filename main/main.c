#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/cap_soil.h"
#include "include/rgb_led.h"
#include "include/soil_control.h"
#include "include/webserver.h"
#include "nvs_flash.h"

static const char *TAG = "MAIN";

#define WIFI_AP_SSID "SoilMonitor"
#define WIFI_AP_PASSWORD "12345678"
#define WIFI_AP_MAX_CONN 4

static void wifi_init_softap(void) {
  esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  wifi_config_t wifi_config = {
      .ap =
          {
              .ssid = WIFI_AP_SSID,
              .ssid_len = strlen(WIFI_AP_SSID),
              .password = WIFI_AP_PASSWORD,
              .max_connection = WIFI_AP_MAX_CONN,
              .authmode = WIFI_AUTH_WPA_WPA2_PSK,
          },
  };
  if (strlen(WIFI_AP_PASSWORD) == 0)
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "WiFi AP started: SSID=%s", WIFI_AP_SSID);
}

void app_main(void) {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ESP_ERROR_CHECK(nvs_flash_init());
  }

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  cap_soil_init();
  rgb_led_init();
  soil_control_init();

  wifi_init_softap();
  start_webserver();

  ESP_LOGI(TAG, "System ready");

  while (1) {
    soil_control_update_leds();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
