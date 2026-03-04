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

  for (size_t i = 0; i < program.size(); i++)
  {
    const auto &tokens = program[i];

    if (tokens.empty())
      continue;

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

    case TokenType::LOOP:
      i = executeLoop(program, i);
      break;

    case TokenType::DLOOP:
      // marcador, no hacer nada
      break;

    default:
      ESP_LOGW("Interpreter", "Unknown command");
      break;
    }
  }
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
    ESP_LOGE("Interpreter", "Invalid write syntax");
    return;
  }

  std::string name = tokens[2].value;
  std::string state = tokens[3].value;

  if (outputs.find(name) == outputs.end())
  {
    ESP_LOGE("Interpreter", "Output %s not found", name.c_str());
    return;
  }

  OutputPin *led = outputs[name];

  ESP_LOGI("Interpreter", "Writing %s to %s",
           state.c_str(), name.c_str());

  if (state == "ON" || state == "on")
    led->on();
  else if (state == "OFF" || state == "off")
    led->off();
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

  // Ejecutar bloque N veces
  for (int r = 0; r < repeatCount; r++)
  {
    ESP_LOGI("Interpreter", "Loop iteration %d/%d", r + 1, repeatCount);

    for (size_t j = loopStart; j < loopEnd; j++)
    {
      executeSingle(program[j]);
    }
  }

  ESP_LOGI("Interpreter", "Loop finished");

  // devolver índice del DLOOP para que el for principal continúe después
  return loopEnd;
}
