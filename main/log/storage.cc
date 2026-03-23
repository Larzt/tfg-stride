#include "storage.h"
#include "esp_log.h"
#include <stdio.h>

void appendToFile(const std::string &filename, const std::string &text)
{
  std::string fullPath = "/sdcard/" + filename;

  FILE *f = fopen(fullPath.c_str(), "a");

  if (f == NULL)
  {
    ESP_LOGE("STORAGE", "Error abriendo archivo: %s", fullPath.c_str());
    return;
  }

  fprintf(f, "%s", text.c_str());
  fclose(f);
}
