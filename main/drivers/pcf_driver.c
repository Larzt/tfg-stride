#include "pcf_driver.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_FREQ_HZ 100000

static uint8_t i2c_addr = 0x27;
static uint8_t pcf_state = 0xFF; // Todos HIGH al inicio

void i2c_scan()
{
  uint8_t i;
  esp_err_t espRc;
  printf("Scanning I2C bus...\n");
  for (i = 0; i < 128; i++)
  {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, true);
    i2c_master_stop(cmd);
    espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    if (espRc == ESP_OK)
    {
      printf("Found device at 0x%02x\n", i);
    }
  }
}

esp_err_t pcf8574_init(uint8_t addr)
{
  i2c_addr = addr;

  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_MASTER_SDA_IO,
      .scl_io_num = I2C_MASTER_SCL_IO,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = I2C_MASTER_FREQ_HZ};

  i2c_param_config(I2C_MASTER_NUM, &conf);
  i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);

  // Scanear el I2C para comprobar la direccion
  i2c_scan();

  // Inicializar todos los pines HIGH
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (i2c_addr << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, pcf_state, true);
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
  i2c_cmd_link_delete(cmd);

  return ret;
}

static esp_err_t pcf_write_state()
{
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (i2c_addr << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, pcf_state, true);
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
  i2c_cmd_link_delete(cmd);
  return ret;
}

esp_err_t pcf8574_led_init(uint8_t pin)
{
  // No es necesario configurar pin como OUTPUT en PCF8574
  return ESP_OK;
}

esp_err_t pcf8574_led_on(uint8_t pin)
{
  pcf_state &= ~(1 << pin); // 0 = LED encendido
  return pcf_write_state();
}

esp_err_t pcf8574_led_off(uint8_t pin)
{
  pcf_state |= (1 << pin); // 1 = LED apagado
  return pcf_write_state();
}

esp_err_t pcf8574_led_toggle(uint8_t pin)
{
  pcf_state ^= (1 << pin);
  return pcf_write_state();
}

int pcf8574_led_get_state(uint8_t pin)
{
  return !(pcf_state & (1 << pin)); // 0 = encendido, 1 = apagado
}
