#include "card_task.hpp"

#include <vector>

void read_card(void *pvParameters)
{
  // std::vector<std::vector<Token>> programBuffer;

  // while (true)
  // {
  //   ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

  //   interpreter.stop_loop();


  //   std::string current_program = Blackboard::MountPoint + Blackboard::CurrentLoadProgram;
  //   FILE *f = fopen(current_program.c_str(), "r");
  //   if (!f)
  //   {
  //     StrideLogger::Warning(StrideSubsystem::Card, "Could not load program file: %s", current_program);
  //     continue;
  //   }

  //   StrideLogger::Log(StrideSubsystem::Card, "Loading program file: %s", current_program);
  //   programBuffer.clear();

  //   char line[128];
  //   while (fgets(line, sizeof(line), f))
  //   {
  //     auto tokens = Tokenize(line);
  //     if (!tokens.empty())
  //     {
  //       programBuffer.push_back(tokens);
  //     }
  //   }

  //   fclose(f);

  //   StrideLogger::Log(StrideSubsystem::Card, "Program loadded succesfully (%d lines)", programBuffer.size());

  //   interpreter.execute(programBuffer);
  // }
}
