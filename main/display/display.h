#pragma once
#include <vector>
#include <string>
#include <dirent.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#define TFT_RST 4
#define TFT_RS 2
#define TFT_CS 15
#define TFT_SDI 13
#define TFT_CLK 14

enum TFT_DIRECTION
{
  Vertical = 0,
  Horizontal = 1,
  InvertedVertical = 2,
  InvertedHorizontal = 3,
};

enum class DisplayState
{
  STARTUP,
  CONFIG_AP,
  FILESYSTEM,
  EXECUTING
};

// Display 176x220
class LGFX_Config : public lgfx::LGFX_Device
{
  lgfx::Panel_ILI9225 _panel_instance;
  lgfx::Bus_SPI _bus_instance;

public:
  LGFX_Config();
};

class Display
{
public:
  static Display &Instance()
  {
    static Display instance;
    return instance;
  }

  void setMode(DisplayState mode) { _currentState = mode; }
  DisplayState getMode() { return _currentState; }

  void begin();
  void showWelcomeScreen();
  void updateStatus(const char *status);
  void showFilesystem(const char *path = "/sdcard");
  void moveSelection(int delta);
  std::string getSelectedFile();
  void showAPInfo(const char *ip_address);
  void clear();

private:
  bool hasExtension(const std::string &filename, const std::string &ext);

  Display() = default;
  LGFX_Config _tft;
  DisplayState _currentState = DisplayState::STARTUP;

  std::vector<std::string> _fileList;
  int _selectedIndex = 0;
};
