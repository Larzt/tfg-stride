#include "interpreter.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// usar std::map<TokenType, std::function<void(const std::vector<Token>&)>> dispatch;

void Interpreter::execute(const std::vector<std::vector<Token>> &program)
{
  if (program.empty())
  {
    ESP_LOGW("Interpreter", "Program is empty");
    return;
  }

  for (size_t index = 0; index < program.size(); index++)
  {
    const auto &tokens = program[index];

    if (tokens.empty())
      continue;

    if (tokens.size() == 3)
    {
      if (tokens[1].type == TokenType::ARROW)
      {
        executeArrow(tokens);
        continue;
      }
      else if (tokens[1].type == TokenType::EQUAL)
      {
        // executeArrow(tokens);
        continue;
      }
    }

    switch (tokens[0].type)
    {
    case TokenType::OUTPUT:
      executeOutput(tokens);
      break;

    case TokenType::WRITE:
      executeWrite(tokens);
      break;

    case TokenType::WAIT:
      executeWait(tokens);
      break;

    case TokenType::ARROW:
      executeArrow(tokens);
      break;

    case TokenType::LOOP:
      index = executeLoop(program, index);
      break;

    case TokenType::IF:
    {
      index = executeIf(program, index);
    }
    break;

    default:
    {
      std::stringstream ss;
      ss << tokens[0].type; // esto usará tu operator<<
      ESP_LOGW("Interpreter", "Unknown command. Type=%s, Value='%s'",
               ss.str().c_str(), tokens[0].value.c_str());
      break;
    }
    }
  }
}

void Interpreter::executeArrow(const std::vector<Token> &tokens)
{
  ESP_LOGI("Interpreter", "Arrow assignment detected");

  // Sintaxis esperada: <value> -> <var>
  if (tokens.size() < 3)
  {
    std::string found = "";
    for (const auto &t : tokens)
    {
      found += t.value + " ";
    }

    ESP_LOGE("Interpreter", "Arrow syntax error: se esperaba <value> -> <var>, se encontró: %s", found.c_str());
    return;
  }

  std::string valueStr = tokens[0].value;
  std::string varName = tokens[2].value;

  char *endPtr = nullptr;
  long value = std::strtol(valueStr.c_str(), &endPtr, 10);

  if (endPtr == valueStr.c_str() || *endPtr != '\0')
  {
    ESP_LOGE("Interpreter", "Arrow assignment: valor no válido: %s", valueStr.c_str());
    return;
  }

  variables[varName] = value;

  ESP_LOGI("Interpreter", "Arrow assignment: %d -> %s", value, varName.c_str());
}

void Interpreter::executeSingle(const std::vector<Token> &tokens)
{
  if (tokens.empty())
    return;

  switch (tokens[0].type)
  {
  case TokenType::OUTPUT:
    executeOutput(tokens);
    break;

  case TokenType::WRITE:
    executeWrite(tokens);
    break;

  case TokenType::WAIT:
    executeWait(tokens);
    break;

  default:
    ESP_LOGW("Interpreter", "Unknown command inside block");
    break;
  }
}

void Interpreter::executeOutput(const std::vector<Token> &tokens)
{
  std::string name;
  int pin = -1;

  for (size_t i = 0; i < tokens.size(); i++)
  {
    if (tokens[i].type == TokenType::NAME && i + 2 < tokens.size())
      name = tokens[i + 2].value;

    if (tokens[i].type == TokenType::PIN && i + 2 < tokens.size())
      pin = std::stoi(tokens[i + 2].value);
  }

  if (!name.empty() && pin >= 0)
  {
    ESP_LOGI("Interpreter", "Creating LED %s on pin %d",
             name.c_str(), pin);

    OutputPin *led = new OutputPin((gpio_num_t)pin);
    led->init();
    outputs[name] = led;
  }
  else
  {
    ESP_LOGE("Interpreter", "Invalid output declaration");
  }
}

void Interpreter::executeWrite(const std::vector<Token> &tokens)
{
  if (tokens.size() < 4)
  {
    ESP_LOGE("Interpreter WRITE", "Invalid write syntax");
    return;
  }

  std::string name = tokens[2].value;
  std::string state = tokens[3].value;

  if (outputs.find(name) == outputs.end())
  {

    ESP_LOGE("Interpreter WRITE", "Output %s not found", name.c_str());

    return;
  }

  OutputPin *led = outputs[name];

  ESP_LOGI("Interpreter", "Writing %s to %s", state.c_str(), name.c_str());

  if (state == "on")
  {
    led->on();
  }
  else if (state == "off")
  {
    led->off();
  }
}

void Interpreter::executeWait(const std::vector<Token> &tokens)
{
  if (tokens.size() < 2)
  {
    ESP_LOGE("Interpreter", "Invalid WAIT syntax");
    return;
  }

  float seconds = std::stof(tokens[1].value);

  ESP_LOGI("Interpreter", "Waiting %.2f seconds...", seconds);

  vTaskDelay(pdMS_TO_TICKS(seconds * 1000));
}

size_t Interpreter::executeLoop(const std::vector<std::vector<Token>> &program, size_t currentIndex)
{
  const auto &tokens = program[currentIndex];

  if (tokens.size() < 2)
  {
    ESP_LOGE("Interpreter", "Invalid LOOP syntax");
    return currentIndex;
  }

  int repeatCount = std::stoi(tokens[1].value);

  ESP_LOGI("Interpreter", "Starting LOOP x%d", repeatCount);

  size_t loopStart = currentIndex + 1;
  size_t loopEnd = loopStart;

  // Buscar DLOOP
  while (loopEnd < program.size() && program[loopEnd][0].type != TokenType::DLOOP)
  {
    loopEnd++;
  }

  if (loopEnd >= program.size())
  {
    ESP_LOGE("Interpreter", "DLOOP not found");
    return currentIndex;
  }

  if (repeatCount == -1)
  {
    start_loop();
    ESP_LOGI("Interpreter", "Endless Loop");
    while (endlessLoop.load())
    {
      for (size_t j = loopStart; j < loopEnd; j++)
      {
        if (!endlessLoop.load())
        {
          break;
        }

        executeSingle(program[j]);
      }
    }
  }
  else
  {

    // Ejecutar bloque N veces
    for (int r = 0; r < repeatCount; r++)
    {
      ESP_LOGI("Interpreter", "Loop iteration %d/%d", r + 1, repeatCount);

      for (size_t j = loopStart; j < loopEnd; j++)
      {
        executeSingle(program[j]);
      }
    }
  }
  ESP_LOGI("Interpreter", "Loop finished");

  // devolver índice del DLOOP para que el for principal continúe después
  return loopEnd;
}

size_t Interpreter::executeIf(const std::vector<std::vector<Token>> &program, size_t currentIndex)
{

  const auto &tokens = program[currentIndex];

  if (tokens.size() < 2)
  {
    ESP_LOGE("Interpreter IF", "IF without condition at line %zu", currentIndex + 1);
    return currentIndex;
  }

  std::string condVar = tokens[1].value;
  std::string condState = tokens[2].value;

  ESP_LOGE("Interpreter IF", "Variable value: %s", condVar.c_str());
  ESP_LOGE("Interpreter IF", "Statement value: %s", condState.c_str());

  bool conditionMatch = false;

  if (outputs.find(condVar) != outputs.end())
  {
    OutputPin *out = outputs[condVar];
    conditionMatch = (condState == "on" && out->get() == 1) ||
                     (condState == "off" && out->get() == 0);
  }
  else if (variables.find(condVar) != variables.end())
  {
    int varValue = variables[condVar];
    int condValue = std::stoi(condState);
    conditionMatch = varValue == condValue;
  }
  else
  {
    ESP_LOGW("Interpreter IF", "Condición desconocida: %s", condVar.c_str());
  }

  size_t blockStart = currentIndex + 1;
  size_t blockEnd = blockStart;
  size_t elseIndex = SIZE_MAX;

  // Buscar ELSE o ENDIF
  while (blockEnd < program.size())
  {
    TokenType t = program[blockEnd][0].type;
    if (t == TokenType::ELSE)
    {
      elseIndex = blockEnd;
    }
    else if (t == TokenType::ENDIF)
    {
      break;
    }
    blockEnd++;
  }

  if (blockEnd >= program.size())
  {
    ESP_LOGE("Interpreter IF", "ENDIF not found for IF at line %zu", currentIndex + 1);
    return currentIndex;
  }

  // Ejecutar el bloque correspondiente
  if (conditionMatch)
  {
    size_t execEnd = (elseIndex != SIZE_MAX) ? elseIndex : blockEnd;
    for (size_t i = blockStart; i < execEnd; i++)
    {
      executeSingle(program[i]);
    }
  }
  else
  {
    if (elseIndex != SIZE_MAX)
    {
      for (size_t i = elseIndex + 1; i < blockEnd; i++)
      {
        executeSingle(program[i]);
      }
    }
  }

  // Devolver índice de ENDIF para que el for principal continue
  return blockEnd;

  return size_t();
}
