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
#include "display.h"
#include "input.h"
#include "output.h"
#include "main.h"

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
   TAREA DEL BOTÓN (determina el modo del servidor)
   ============================================================ */
void button_task(void *pvParameters)
{
    InputPin mode_button(BUTTON_MODE);
    OutputPin mode_led(LED_MODE, true);
    mode_button.init();
    mode_led.init();

    while (true)
    {
        auto &display = Display::Instance();

        if (mode_button.wait_for_long_press(TIME_PRESSED))
        {
            mode_led.toggle();
            if (mainTaskHandle != NULL)
            {
                xTaskNotify(mainTaskHandle, mode_led.get(), eSetValueWithOverwrite);
            }
            // Esperar a que suelte para no detectar clicks accidentales después
            while (mode_button.is_pressed())
                vTaskDelay(pdMS_TO_TICKS(10));
        }
        else if (mode_button.just_pressed())
        {
            if (display.getMode() == DisplayState::FILESYSTEM)
            {
                display.moveSelection(1);
                ESP_LOGI("Button", "Click corto: Moviendo selector");
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
        auto &display = Display::Instance();

        if (reloadButton.wait_for_long_press(TIME_PRESSED))
        {
            ESP_LOGI("BUTTON", "Ejecutando archivo seleccionado...");

            // Si el intérprete estaba corriendo algo, lo paramos
            Interpreter::Instance().stop_loop();

            if (sdReadTaskHandle != NULL)
            {
                xTaskNotifyGive(sdReadTaskHandle); // Despertamos a sd_read_task
            }

            while (reloadButton.is_pressed())
                vTaskDelay(pdMS_TO_TICKS(10));
        }
        else if (reloadButton.just_pressed())
        {
            if (display.getMode() == DisplayState::FILESYSTEM)
            {
                display.moveSelection(-1);
                ESP_LOGI("Button", "Click corto: Moviendo selector");
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
    auto &display = Display::Instance();

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
    auto &display = Display::Instance();

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

    // display.updateStatus("Cargando archivos...")
    ESP_LOGI("SD", "SD montada correctamente");
    display.setMode(DisplayState::FILESYSTEM);

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

/* ============================================================
   TFT Display 176x220
   ============================================================ */

void tft_task(void *pvParameters)
{
    auto &display = Display::Instance();
    display.begin();
    display.showWelcomeScreen();

    vTaskDelay(pdMS_TO_TICKS(1000));

    while (true)
    {
        switch (display.getMode())
        {
        case DisplayState::STARTUP:
            break;

        case DisplayState::CONFIG_AP:
            break;

        case DisplayState::FILESYSTEM:
            display.showFilesystem();
            break;

        case DisplayState::EXECUTING:
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
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
        4096,
        NULL,
        4,
        NULL,
        1);

    // Tarea Reload de la app
    xTaskCreatePinnedToCore(
        button_reload_task,
        "ButtonReloadTask",
        4096,
        NULL,
        5,
        NULL,
        1);
}
