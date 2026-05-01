#include "display.h"
#include "esp_log.h"

static const char *TAG = "DisplayManager";

LGFX_Config::LGFX_Config()
{
  {
    auto cfg = _bus_instance.config();
    cfg.spi_host = SPI3_HOST;
    cfg.spi_mode = 0;
    cfg.freq_write = 10000000;
    cfg.pin_sclk = TFT_CLK;
    cfg.pin_mosi = TFT_SDI;
    cfg.pin_miso = -1;
    cfg.pin_dc = TFT_RS;
    _bus_instance.config(cfg);
    _panel_instance.setBus(&_bus_instance);
  }
  {
    auto cfg = _panel_instance.config();
    cfg.pin_cs = TFT_CS;
    cfg.pin_rst = TFT_RST;
    cfg.panel_width = 176;
    cfg.panel_height = 220;
    cfg.bus_shared = false;
    _panel_instance.config(cfg);
  }
  setPanel(&_panel_instance);
}

void Display::begin()
{
  ESP_LOGI(TAG, "Inicializando pantalla...");
  _tft.init();
  _tft.setRotation(TFT_DIRECTION::Vertical);
  _tft.fillScreen(TFT_BLACK);
}

void Display::showWelcomeScreen()
{
  _tft.fillScreen(TFT_BLACK);
  _tft.setTextColor(TFT_YELLOW);
  _tft.setTextSize(2);
  _tft.setCursor(10, 80);
  _tft.println("Bienvenido a");
  _tft.setCursor(50, 100);
  _tft.println("Stride");
  _tft.drawLine(0, 120, 176, 120, TFT_GREEN);
}

void Display::updateStatus(const char *status)
{
  _tft.fillRect(0, 130, 176, 20, TFT_BLACK);
  _tft.setCursor(10, 130);
  _tft.setTextSize(1);
  _tft.setTextColor(TFT_WHITE);
  _tft.print(status);
}

void Display::showFilesystem(const char *path)
{
  // 1. Escanear archivos solo si la lista está vacía o queremos refrescar
  // (Por ahora lo hacemos cada vez para simplificar)
  _fileList.clear();
  DIR *dir = opendir(path);
  if (dir)
  {
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
      std::string name = entry->d_name;
      if (hasExtension(name, ".str") || hasExtension(name, ".STR") || hasExtension(name, ".log") || hasExtension(name, ".LOG"))
      {
        _fileList.push_back(name);
      }
    }
    closedir(dir);
  }

  // 2. Dibujar en pantalla
  _tft.fillScreen(TFT_BLACK);
  _tft.setCursor(0, 0); // Reset cursor al inicio para el título
  _tft.setTextColor(TFT_MAGENTA);
  _tft.setTextSize(2);
  _tft.println(" SELECTOR STR");
  _tft.drawLine(0, 20, 176, 20, TFT_WHITE);

  // 3. Reset cursor para la lista de archivos
  _tft.setCursor(0, 25); // <--- ESTO ES VITAL
  _tft.setTextSize(1);

  for (int i = 0; i < _fileList.size(); ++i)
  {
    // Limitar el número de archivos para que no desborde la pantalla (opcional pero recomendado)
    if (_tft.getCursorY() > 210)
      break;

    if (i == _selectedIndex)
    {
      _tft.setTextColor(TFT_BLACK, TFT_CYAN);
      _tft.printf("> %s \n", _fileList[i].c_str());
    }
    else
    {
      _tft.setTextColor(TFT_WHITE, TFT_BLACK);
      _tft.printf("  %s \n", _fileList[i].c_str());
    }
  }
}

void Display::moveSelection(int delta)
{
  if (_fileList.empty())
    return;

  _selectedIndex += delta;

  // Control de límites (Circular)
  if (_selectedIndex < 0)
    _selectedIndex = _fileList.size() - 1;
  if (_selectedIndex >= _fileList.size())
    _selectedIndex = 0;

  showFilesystem(); // Refrescamos la pantalla
}

std::string Display::getSelectedFile()
{
  if (_selectedIndex >= 0 && _selectedIndex < _fileList.size())
  {
    return "/sdcard/" + _fileList[_selectedIndex];
  }
  return "";
}

void Display::showAPInfo(const char *ip_address)
{
  _tft.fillScreen(TFT_BLACK);
  _tft.setTextColor(TFT_WHITE);

  _tft.setTextSize(2);
  _tft.setCursor(10, 20);
  _tft.println("Configuracion");
  _tft.drawLine(0, 45, 176, 45, TFT_BLUE);

  // Instrucciones
  _tft.setTextSize(1);
  _tft.setCursor(10, 60);
  _tft.println("Conectate a:");
  _tft.setTextColor(TFT_YELLOW);
  _tft.setCursor(10, 75);
  _tft.println("ESP32_Config"); // Cambia esto al nombre de tu SSID

  _tft.setTextColor(TFT_WHITE);
  _tft.setCursor(10, 110);
  _tft.println("Y entra en la URL:");

  // IP Address destacada
  _tft.setTextColor(TFT_GREEN);
  _tft.setTextSize(2);
  _tft.setCursor(10, 130);
  _tft.println(ip_address);

  ESP_LOGI(TAG, "Mostrando IP de configuración: %s", ip_address);
}

bool Display::hasExtension(const std::string &filename, const std::string &ext)
{
  if (filename.length() < ext.length())
  {
    return false;
  }
  return filename.compare(filename.length() - ext.length(), ext.length(), ext) == 0;
}
