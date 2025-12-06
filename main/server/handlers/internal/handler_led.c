#include "handler_led.h"
#include "drivers/led_driver.h"
#include "config/routes.h"

static const int LED_PIN = 26;

esp_err_t put_pin_led(httpd_req_t *req)
{
  led_init(LED_PIN);
  led_toggle();

  char resp[128];
  snprintf(resp, sizeof(resp),
           "{"
           "\"pin\": %d,"
           "\"state\": %d"
           "}",
           LED_PIN,
           led_get_state());

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

  return ESP_OK;
}

httpd_uri_t uri_led = {
    .uri = PUT_LED,
    .method = HTTP_PUT,
    .handler = put_pin_led,
    .user_ctx = NULL};
