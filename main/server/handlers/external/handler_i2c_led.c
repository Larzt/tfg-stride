#include "handler_i2c_led.h"
#include "pcf_driver.h"
#include "config/routes.h"

static const int LED_PIN = 0; // P0 del PCF8574T

esp_err_t put_pin_i2c_led(httpd_req_t *req)
{
  pcf8574_led_init(LED_PIN);
  pcf8574_led_toggle(LED_PIN);

  char resp[128];
  snprintf(resp, sizeof(resp),
           "{"
           "\"pin\": %d,"
           "\"state\": %d"
           "}",
           LED_PIN,
           pcf8574_led_get_state(LED_PIN));

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

  return ESP_OK;
}

httpd_uri_t uri_i2c_led = {
    .uri = PUT_I2C_LED,
    .method = HTTP_PUT,
    .handler = put_pin_i2c_led,
    .user_ctx = NULL};
