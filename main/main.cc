#include "network.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "interpreter.h"

// SD Module & SPI
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"

// Server & Handlers
#include "server.h"

// Controllers
#include "input.h"
#include "output.h"
#include "main.h"

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#define TFT_RST 4  // Pin RST
#define TFT_RS 2   // Pin RS
#define TFT_CS 15  // Pin CS
#define TFT_SDI 13 // Data In / MOSI
#define TFT_CLK 14 // Reloj / SCK

// ============================================================
// CONFIGURACIÓN DE PANTALLA LOVYANGFX (ILI9225)
// ============================================================
class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9225 _panel_instance;
    lgfx::Bus_SPI _bus_instance;

public:
    LGFX(void)
    {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host = SPI3_HOST;  // Usamos el HSPI (aislado de la SD)
            cfg.spi_mode = 0;          // Modo SPI
            cfg.freq_write = 10000000; // Velocidad: 10 MHz
            cfg.pin_sclk = TFT_CLK;    // GPIO 14
            cfg.pin_mosi = TFT_SDI;    // GPIO 13
            cfg.pin_miso = -1;         // No usamos MISO en la pantalla
            cfg.pin_dc = TFT_RS;       // GPIO 2
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs = TFT_CS;    // GPIO 15
            cfg.pin_rst = TFT_RST;  // GPIO 4
            cfg.panel_width = 176;  // Ancho del ILI9225
            cfg.panel_height = 220; // Alto del ILI9225
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.bus_shared = false; // No compartimos este bus con nadie más
            _panel_instance.config(cfg);
        }
        setPanel(&_panel_instance);
    }
};

// Creamos la instancia global de la pantalla
LGFX tft;

// TIME
// #include <time.h>
// #include <sys/time.h>
// #include "esp_netif_sntp.h"
// #include "esp_sntp.h"

#define TIME_PRESSED 3000

#define LED_MODE GPIO_NUM_26
#define BUTTON_MODE GPIO_NUM_16
#define BUTTON_RELOAD GPIO_NUM_32

// Handle solo de la tarea principal
static TaskHandle_t mainTaskHandle = NULL;
static TaskHandle_t sdReadTaskHandle = NULL;

/* ============================================================
   TAREA PRINCIPAL (servidor - pesada)
   ============================================================ */
void main_task(void *pvParameters)
{
    Server *server = (Server *)pvParameters;
    uint32_t led_state;

    ESP_LOGI("SYSTEM", "MainTask iniciada");

    while (true)
    {
        if (xTaskNotifyWait(0, 0, &led_state, portMAX_DELAY))
        {
            ESP_LOGI("SYSTEM", "Cambio de modo recibido");

            // LED off when is dev mode
            if (led_state == 0)
            {
                server->set_dev_mode(DEV);
                ESP_LOGI("SYSTEM", "Modo DEV activado");
            }
            else
            {
                server->set_dev_mode(USER);
                ESP_LOGI("SYSTEM", "Modo USER activado");
            }

            server->reset_handlers();
            ESP_LOGI("SYSTEM", "Servidor reconfigurado");
        }
    }
}

/* ============================================================
   TAREA DEL BOTÓN (solo controla servidor)
   ============================================================ */
void button_task(void *pvParameters)
{
    InputPin mode_button(BUTTON_MODE);
    OutputPin mode_led(LED_MODE, true);

    mode_button.init();
    mode_led.init();

    ESP_LOGI("SYSTEM", "ButtonTask iniciada");

    while (true)
    {
        if (mode_button.wait_for_long_press(TIME_PRESSED))
        {
            ESP_LOGW("Inputs", "Five seconds passed!");

            mode_led.toggle();

            if (mainTaskHandle != NULL)
            {
                xTaskNotify(
                    mainTaskHandle,
                    mode_led.get(),
                    eSetValueWithOverwrite);
            }

            ESP_LOGI("Button", "Modo cambiado");

            while (mode_button.is_pressed())
            {
                vTaskDelay(pdMS_TO_TICKS(50));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/* ============================================================
   TAREA REINICIO INTERPRETER (INDEPENDIENTE)
   ============================================================ */
void button_reload_task(void *pvParameters)
{
    InputPin reloadButton(BUTTON_RELOAD);
    reloadButton.init();

    while (true)
    {
        if (reloadButton.wait_for_long_press(TIME_PRESSED))
        {
            ESP_LOGI("BUTTON", "Recargando programa");

            Interpreter::Instance().stop_loop();
            if (sdReadTaskHandle != NULL)
            {
                xTaskNotifyGive(sdReadTaskHandle);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/* ============================================================
   TAREA TEST INTERPRETER (INDEPENDIENTE)
   ============================================================ */
void sd_read_task(void *pvParameters)
{
    auto &interpreter = Interpreter::Instance();

    std::vector<std::vector<Token>> programBuffer;

    while (true)
    {
        ESP_LOGI("SD", "Esperando señal para ejecutar programa...");

        // Aquí la tarea se duerme completamente
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        interpreter.stop_loop(); // rompe loops infinitos si existen

        ESP_LOGI("SD", "Leyendo archivo program.str...");

        FILE *f = fopen("/sdcard/program.str", "r");
        if (!f)
        {
            ESP_LOGW("SD", "Archivo no encontrado");
            continue;
        }

        programBuffer.clear();

        char line[128];
        while (fgets(line, sizeof(line), f))
        {
            auto tokens = Tokenize(line);
            if (!tokens.empty())
                programBuffer.push_back(tokens);
        }

        fclose(f);

        ESP_LOGI("SD", "Programa cargado (%d lineas)", programBuffer.size());

        interpreter.execute(programBuffer);

        ESP_LOGI("SD", "Programa terminado");
    }
}

/* ============================================================
TAREA SD (INDEPENDIENTE)
============================================================ */
void sd_task(void *pvParameters)
{
    ESP_LOGI("SD", "Iniciando SD task...");

    esp_err_t ret;

    // Configuración del host SPI
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    spi_bus_config_t bus_cfg = {};
    bus_cfg.mosi_io_num = GPIO_NUM_23;
    bus_cfg.miso_io_num = GPIO_NUM_19;
    bus_cfg.sclk_io_num = GPIO_NUM_18;
    bus_cfg.quadwp_io_num = -1;
    bus_cfg.quadhd_io_num = -1;

    ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SDSPI_DEFAULT_DMA);

    if (ret == ESP_ERR_INVALID_STATE)
    {
        ESP_LOGW("SD", "SPI ya estaba inicializado, continuando...");
        ret = ESP_OK;
    }

    if (ret != ESP_OK)
    {
        ESP_LOGE("SD", "Error inicializando SPI");
        vTaskDelete(NULL);
        return;
    }

    if (ret != ESP_OK)
    {
        ESP_LOGE("SD", "Error inicializando bus SPI");
        vTaskDelete(NULL);
        return;
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = GPIO_NUM_5;
    slot_config.host_id = SPI2_HOST;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
        .disk_status_check_enable = true,
    };

    sdmmc_card_t *card;

    ret = esp_vfs_fat_sdspi_mount("/sdcard", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK)
    {
        ESP_LOGE("SD", "No se pudo montar la SD");
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI("SD", "SD montada correctamente");

    if (sdReadTaskHandle != NULL)
    {
        xTaskNotifyGive(sdReadTaskHandle);
    }

    vTaskDelete(NULL);
}

/* ============================================================
   INITIALIZE TIME (logs) !!Not working
   ============================================================ */
// void initTime()
// {
//     ESP_LOGI("TIME", "Inicializando SNTP...");

//     // Configuración para IDF 5.x
//     esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");

//     // Inicialización
//     esp_netif_sntp_init(&config);

//     // Esperar sincronización (opcional, máximo 10s)
//     if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000)) != ESP_OK)
//     {
//         ESP_LOGW("TIME", "No se pudo sincronizar la hora");
//     }
//     else
//     {
//         ESP_LOGI("TIME", "Hora sincronizada");
//     }
// }

enum TFT_DIRECTION {
    Vertical = 0,
    Horizontal = 1,
    InvertedVertical = 2,
    InvertedHorizontal = 3,
};
/* ============================================================
   TAREA PANTALLA TFT 176x220
   ============================================================ */
void tft_task(void *pvParameters)
{
    ESP_LOGI("TFT", "Inicializando LovyanGFX...");
    tft.init();

    ESP_LOGI("TFT", "Init superado. Pintando pantalla de azul...");
    tft.setRotation(TFT_DIRECTION::Vertical);
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(2);
    tft.setCursor(10, 80); // x, y
    tft.println("Bienvenido a");
    tft.setCursor(50, 100);
    tft.println("Stride");

    tft.drawLine(0, 120, 176, 120, TFT_GREEN);

    ESP_LOGI("TFT", "Dibujo completado.");
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/* ============================================================
   APP MAIN
   ============================================================ */
extern "C" void app_main(void)
{
    Network wifi;
    wifi.init();

    // initTime();

    Server *server = new Server();
    server->start();

    // Tarea servidor
    xTaskCreatePinnedToCore(
        main_task,
        "MainTask",
        6144,
        (void *)server,
        5,
        &mainTaskHandle,
        1);

    // Tarea botón
    xTaskCreatePinnedToCore(
        button_task,
        "ButtonTask",
        4096,
        NULL,
        5,
        NULL,
        1);

    // Tarea SD (monta la SD)
    xTaskCreatePinnedToCore(
        sd_task,
        "SDTask",
        6144,
        NULL,
        4,
        NULL,
        1);

    // Tarea que lee el contenido de los archivos solo cuando SD esté lista
    xTaskCreatePinnedToCore(
        sd_read_task,
        "SDReadTask",
        4096,
        NULL,
        4,
        &sdReadTaskHandle,
        1);

    // Tarea Pantalla TFT
    xTaskCreatePinnedToCore(
        tft_task,
        "TFTTask",
        4096, // Ajusta este tamaño de pila si tu librería gráfica lo necesita
        NULL,
        4, // Prioridad
        NULL,
        1 // Ejecutando en el Core 1 (junto con la SD y main)
    );

    xTaskCreatePinnedToCore(
        button_reload_task,
        "ButtonReloadTask",
        4096,
        NULL,
        5,
        NULL,
        1);
}
