#include "server_mode_task.hpp"

void hear_server_mode_button(void *pvParameters)
{
  StrideButton button(Blackboard::ModeButton);

  while (true)
  {

    if (button.wait_for_long_press(Blackboard::ModeTimePressed))
    {
      StrideLogger::Log(StrideSubsystem::Server, "Currently pressed for long time mode button");
      Blackboard::CurrentServerMode = Blackboard::CurrentServerMode.get() == ServerMode::Developer
      ? ServerMode::Production : ServerMode::Developer;
    }
    else if (button.just_pressed()) {
      StrideLogger::Log(StrideSubsystem::Server, "Currently pressed mode button");
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
