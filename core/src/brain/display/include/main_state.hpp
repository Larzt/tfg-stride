#pragma once
#include "display.hpp"

#include <dirent.h>
#include <string>
#include <vector>

#include "enums.hpp"
#include "stride_logger.hpp"
#include "stride_observer.hpp"
#include "stride_subscription.hpp"

class MainState : public DisplayBaseState
{
public:
  void on_enter(Display &ctx) override
  {
    // Read card files
    DIR *dir = opendir(Blackboard::MountPoint.c_str());
    if (dir)
    {
      struct dirent *entry;
      while ((entry = readdir(dir)) != NULL)
      {
        std::string name = "/" + std::string(entry->d_name);
        std::string extension = name.substr(name.length() - 4);
        if (extension != ".log" && extension != ".LOG" && extension != ".str" && extension != ".STR")
        {
          continue;
        }
        _files.emplace_back(name);
      }
      closedir(dir);
    }

    auto &tft = ctx.getTFT();
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(2);
    tft.drawCenterString("Programas", tft.width() / 2, 10);
    tft.drawCenterString("cargados", tft.width() / 2, 30);

    draw_files(ctx);

    _cursor_subscription = _cursor_position.subscribe([this, &ctx](int)
                                                      { draw_files(ctx); });
  }

  void on_exit(Display &ctx) override
  {
    _cursor_subscription.unsubscribe();
  }

  void on_update(Display &ctx) override {}

  void on_input(const InputEvent &event) override
  {
    if (event.type == InputType::Button)
    {
      int value = event.delta;
      move_cursor_position(value);
    }
  }

  void draw_files(Display &ctx)
  {
    auto &tft = ctx.getTFT();

    tft.setTextSize(1);

    int y = 50;
    int lineHeight = 10;

    for (size_t i = 0; i < _files.size(); i++)
    {
      if (i == (size_t)_cursor_position.get())
      {
        tft.setTextColor(TFT_ALICEBLUE, TFT_BLACK);
      }
      else
      {
        tft.setTextColor(TFT_BLACK, TFT_YELLOW);
      }

      tft.drawString(_files[i].c_str(), 10, y);
      y += lineHeight;

      if (y > tft.height() - 20)
        break;
    }
  }

  void move_cursor_position(int delta)
  {
    if (_files.empty())
    {
      return;
    }

    _cursor_position.set(_cursor_position.get() + delta);

    if (_cursor_position.get() < 0)
    {
      _cursor_position.set(_files.size() - 1);
    }

    if (_cursor_position.get() >= (int)_files.size())
    {
      _cursor_position.set(0);
    }
  }

  std::string get_name() const override { return "MAIN"; }

private:
  std::vector<std::string> _files;
  StrideObservable<int> _cursor_position{0};
  StrideSubscription _cursor_subscription;
};
