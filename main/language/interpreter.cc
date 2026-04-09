#include "interpreter.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "parseHex.h"
#include "storage.h"
// #include "time_utils.h"

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
        executeEqual(tokens);
        continue;
      }
    }

    switch (tokens[0].type)
    {
    case TokenType::FILE:
      executeLogfile(tokens);
      break;

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

    case TokenType::PRINT:
      executePrint(tokens);
      break;

    case TokenType::I2C:
      executeI2C(tokens);
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
      ss << tokens[0].type;
      ESP_LOGW("Interpreter", "Unknown command. Type=%s, Value='%s'",
               ss.str().c_str(), tokens[0].value.c_str());
      break;
    }
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

  case TokenType::PRINT:
    executePrint(tokens);
    break;

  default:
    ESP_LOGW("Interpreter", "Unknown command inside block");
    break;
  }
}

void Interpreter::executeLogfile(const std::vector<Token> &tokens)
{

  ESP_LOGI("Interpreter FILE", "File assigment detected");

  // Sintaxis esperada: FILE <name>
  if (tokens.size() < 2)
  {
    std::string found = "";
    for (const auto &t : tokens)
    {
      found += t.value + " ";
    }

    ESP_LOGE("Interpreter FILE", "File syntax error: se esperaba FILE <name>, se encontró: %s", found.c_str());
    return;
  }

  std::string fileName = tokens[1].value;

  char *endPtr = nullptr;
  long value = std::strtol(fileName.c_str(), &endPtr, 10);

  if (endPtr == fileName.c_str() || *endPtr != '\0')
  {
    ESP_LOGE("Interpreter FILE", "File assignment: valor no válido: %s", fileName.c_str());
    return;
  }

  logFile = value;

  ESP_LOGI("Interpreter FILE", "Assigned logfile for this program: %s", fileName.c_str());
}

void Interpreter::executeArrow(const std::vector<Token> &tokens)
{
  ESP_LOGI("Interpreter ARROW", "Arrow assignment detected");

  // Sintaxis esperada: <value> -> <var>
  if (tokens.size() < 3)
  {
    std::string found = "";
    for (const auto &t : tokens)
    {
      found += t.value + " ";
    }

    ESP_LOGE("Interpreter ARROW", "Arrow syntax error: se esperaba <value> -> <var>, se encontró: %s", found.c_str());
    return;
  }

  std::string valueStr = "";
  std::string varName = "";

  for (size_t i = 0; i < tokens.size(); i++)
  {
    if (tokens[i].type == TokenType::ARROW)
    {
      valueStr = tokens[i - 1].value;
      varName = tokens[i + 1].value;
    }
  }

  char *endPtr = nullptr;
  long value = std::strtol(valueStr.c_str(), &endPtr, 10);

  if (endPtr == valueStr.c_str() || *endPtr != '\0')
  {
    ESP_LOGE("Interpreter ARROW", "Arrow assignment: valor no válido: %s", valueStr.c_str());
    return;
  }

  variables[varName] = value;

  ESP_LOGI("Interpreter ARROW", "Arrow assignment: %ld -> %s", (long)value, varName.c_str());
}

void Interpreter::executeEqual(const std::vector<Token> &tokens)
{
  ESP_LOGI("Interpreter EQUAL", "Equal assignment detected");

  // Sintaxis esperada: <var> = <value>
  if (tokens.size() < 3)
  {
    std::string found = "";
    for (const auto &t : tokens)
    {
      found += t.value + " ";
    }

    ESP_LOGE("Interpreter EQUAL", "Equal syntax error: se esperaba <var> = <value>, se encontró: %s", found.c_str());
    return;
  }

  std::string varName = tokens[0].value;
  std::string valueStr = tokens[2].value;

  char *endPtr = nullptr;
  long value = std::strtol(valueStr.c_str(), &endPtr, 10);

  if (endPtr == valueStr.c_str() || *endPtr != '\0')
  {
    ESP_LOGE("Interpreter EQUAL", "Equal assignment: valor no válido: %s", valueStr.c_str());
    return;
  }

  variables[varName] = value;

  ESP_LOGI("Interpreter EQUAL", "Equal assignment: %s = %ld", varName.c_str(), (long)value);
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

void Interpreter::executePrint(const std::vector<Token> &tokens)
{
  if (tokens.size() < 2)
  {
    ESP_LOGE("Interpreter PRINT", "PRINT without value");
    return;
  }
  std::string fullOutput = "";
  for (size_t i = 1; i < tokens.size(); i++)
  {
    const Token &t = tokens[i];

    if (t.type == TokenType::STRING)
    {
      fullOutput += t.value;
    }
    else if (variables.find(t.value) != variables.end())
    {
      fullOutput += std::to_string(variables[t.value]);
    }
    else
    {
      fullOutput += t.value;
    }

    if (i + 1 < tokens.size())
      fullOutput += " ";
  }

  std::string finalLine = "[HORA]: " + fullOutput + "\n";
  ESP_LOGI("Interpreter PRINT", "%s", finalLine.c_str());

  appendToFile(logFile, finalLine);
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

void Interpreter::executeI2C(const std::vector<Token> &tokens)
{
  if (tokens.size() < 2)
  {
    ESP_LOGE("Interpreter I2C", "Incomplete I2C command");
    return;
  }

  switch (tokens[1].type)
  {
  case TokenType::INIT:
    executeI2CInit(tokens);
    break;

  case TokenType::WRITE:
    executeI2CWrite(tokens);
    break;

  case TokenType::READ:
    executeI2CRead(tokens);
    break;

  default:
    ESP_LOGW("Interpreter I2C", "Unknown I2C subcommand");
    break;
  }
}

void Interpreter::executeI2CInit(const std::vector<Token> &tokens)
{
  int sda = -1;
  int scl = -1;

  for (size_t i = 0; i < tokens.size(); i++)
  {
    bool limit = i + 1 < tokens.size();
    if (tokens[i].type == TokenType::SDA && limit)
    {
      sda = std::stoi(tokens[i + 1].value);
    }

    if (tokens[i].type == TokenType::SDA && limit)
    {
      scl = std::stoi(tokens[i + 1].value);
    }
  }

  if (sda == -1 || scl == -1)
  {
    ESP_LOGE("Interpreter I2C", "Invalid INIT syntaxis");
    return;
  }

  ESP_LOGI("Interpreter I2C", "Init I2C SDA=%d SCL=%d", sda, scl);

  // Inicializar el I2C con el driver
  // i2c_master_init((gpio_num_t)sda, (gpio_num_t)scl);
}

void Interpreter::executeI2CWrite(const std::vector<Token> &tokens)
{
  if (tokens.size() < 5)
  {
    ESP_LOGE("Interpreter I2C", "Invalid WRITE syntax");
    return;
  }

  int addr = parseNumber(tokens[2].value);
  int reg = parseNumber(tokens[3].value);
  int data = parseNumber(tokens[4].value);

  ESP_LOGI("Interpreter I2C", "WRITE addr=0x%X reg=0x%X data=0x%X",
           addr, reg, data);

  // i2c_write_byte(addr, reg, data);
}

void Interpreter::executeI2CRead(const std::vector<Token> &tokens)
{
  // Sintaxis mínima: I2C READ addr reg bytes
  // Sintaxis opcional: I2C READ addr reg bytes => var
  if (tokens.size() < 5)
  {
    ESP_LOGE("Interpreter I2C", "Invalid READ syntax");
    return;
  }

  int addr = parseNumber(tokens[2].value);
  int reg = parseNumber(tokens[3].value);
  int bytes = std::stoi(tokens[4].value);

  ESP_LOGI("Interpreter I2C", "READ addr=0x%X reg=0x%X bytes=%d", addr, reg, bytes);

  int result = 1234; // placeholder: i2c_read_bytes(addr, reg, bytes);

  std::string varName = "";

  for (size_t i = 0; i < tokens.size(); i++)
  {
    if (tokens[i].type == TokenType::ARROW && i + 1 < tokens.size())
    {
      varName = tokens[i + 1].value;
    }
  }

  if (varName != "")
  {
    variables[varName] = result;
    ESP_LOGI("Interpreter I2C", "Stored %d in %s", result, varName.c_str());
  }
  else
  {
    ESP_LOGI("Interpreter I2C", "Read result (not stored): %d", result);
  }
}
