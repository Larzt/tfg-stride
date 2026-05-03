#include "reload_task.hpp"

void hear_program_reload_button(void *pvParameters)
{
  StrideButton button(Blackboard::ReloadProgramButton);

  while (true)
  {
    if (button.wait_for_long_press(Blackboard::ReloadTimePressed))
    {
      StrideLogger::Log(StrideSubsystem::Interpreter, "Loading selected program...");

      // Interpreter::Instance().stop_loop();

      // if (sdReadTaskHandle != NULL)
      // {
      //   xTaskNotifyGive(sdReadTaskHandle);
      // }

      while (button.is_pressed())
      {
        vTaskDelay(pdMS_TO_TICKS(10));
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
