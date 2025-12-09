#include "webserver.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "soil_control.h"
#include <stdlib.h>
#include <string.h>

static const char *TAG = "WEB";

extern const uint8_t _binary_page_html_gz_start[];
extern const uint8_t _binary_page_html_gz_end[];
extern const uint8_t _binary_style_css_gz_start[];
extern const uint8_t _binary_style_css_gz_end[];
extern const uint8_t _binary_script_js_gz_start[];
extern const uint8_t _binary_script_js_gz_end[];

static esp_err_t root_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html; charset=utf-8");
  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  return httpd_resp_send(req, (const char *)_binary_page_html_gz_start,
                         _binary_page_html_gz_end - _binary_page_html_gz_start);
}

static esp_err_t css_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/css; charset=utf-8");
  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  return httpd_resp_send(req, (const char *)_binary_style_css_gz_start,
                         _binary_style_css_gz_end - _binary_style_css_gz_start);
}

static esp_err_t js_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/javascript; charset=utf-8");
  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  return httpd_resp_send(req, (const char *)_binary_script_js_gz_start,
                         _binary_script_js_gz_end - _binary_script_js_gz_start);
}

static esp_err_t status_handler(httpd_req_t *req) {
  int m1 = soil_control_get_moisture_percent(1);
  int m2 = soil_control_get_moisture_percent(2);
  char json[256];
  snprintf(json, sizeof(json),
           "{\"moisture\":{\"1\":%d,\"2\":%d},\"enabled\":{\"1\":%s,\"2\":%s}}",
           m1, m2, (m1 >= 0) ? "true" : "false", (m2 >= 0) ? "true" : "false");
  httpd_resp_set_type(req, "application/json");
  return httpd_resp_send(req, json, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t control_handler(httpd_req_t *req) {
  char query[128];
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing query");
    return ESP_FAIL;
  }

  soil_settings_t s = soil_control_get_settings();

  char val[8];
  if (httpd_query_key_value(query, "red", val, sizeof(val)) == ESP_OK)
    s.red_threshold = atoi(val);
  if (httpd_query_key_value(query, "green", val, sizeof(val)) == ESP_OK)
    s.green_threshold = atoi(val);
  if (httpd_query_key_value(query, "s1", val, sizeof(val)) == ESP_OK)
    s.sensor1_enabled = (val[0] == '1');
  if (httpd_query_key_value(query, "s2", val, sizeof(val)) == ESP_OK)
    s.sensor2_enabled = (val[0] == '1');
  if (httpd_query_key_value(query, "l1", val, sizeof(val)) == ESP_OK)
    s.led1_enabled = (val[0] == '1');
  if (httpd_query_key_value(query, "l2", val, sizeof(val)) == ESP_OK)
    s.led2_enabled = (val[0] == '1');

  soil_control_set_settings(&s);
  soil_control_update_leds();

  httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

void start_webserver(void) {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  httpd_handle_t server = NULL;
  if (httpd_start(&server, &config) != ESP_OK) {
    ESP_LOGE(TAG, "Failed to start HTTP server");
    return;
  }

  httpd_uri_t uris[] = {{"/", HTTP_GET, root_handler, NULL},
                        {"/style.css", HTTP_GET, css_handler, NULL},
                        {"/script.js", HTTP_GET, js_handler, NULL},
                        {"/status", HTTP_GET, status_handler, NULL},
                        {"/control", HTTP_GET, control_handler, NULL}};

  for (int i = 0; i < sizeof(uris) / sizeof(uris[0]); i++) {
    httpd_register_uri_handler(server, &uris[i]);
  }

  ESP_LOGI(TAG, "HTTP server started");
}