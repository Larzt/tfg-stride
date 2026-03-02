#include "interpreter.h"
#include "esp_log.h"

void Interpreter::execute(const std::vector<Token> &tokens)
{
  if (tokens.empty())
  {
    ESP_LOGW("Interpreter", "No tokens received");
    return;
  }

  ESP_LOGI("Interpreter", "Tokens received: %d", tokens.size());

  // -----------------------------
  // OUTPUT DECLARATION
  // output = led name = myLed pin = 16
  // -----------------------------
  if (tokens[0].type == TokenType::OUTPUT)
  {
    std::string name;
    int pin = -1;

    for (size_t i = 0; i < tokens.size(); i++)
    {
      if (tokens[i].type == TokenType::NAME &&
          i + 2 < tokens.size())
      {
        name = tokens[i + 2].value;
      }

      if (tokens[i].type == TokenType::PIN &&
          i + 2 < tokens.size())
      {
        pin = std::stoi(tokens[i + 2].value);
      }
    }

    if (!name.empty() && pin >= 0)
    {
      ESP_LOGW("Interpreter", "Creating LED %s on pin %d",
               name.c_str(), pin);

      OutputPin *led = new OutputPin((gpio_num_t)pin);
      led->init();
      outputs[name] = led;
    }
    else
    {
      ESP_LOGE("Interpreter", "Invalid output declaration");
    }

    return;
  }

  // -----------------------------
  // WRITE COMMAND
  // write = myLed on
  // -----------------------------
  if (tokens[0].type == TokenType::WRITE)
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

    ESP_LOGW("Interpreter", "Writing %s to %s",
             state.c_str(), name.c_str());

    if (state == "on")
      led->turn(true);
    else if (state == "off")
      led->turn(false);

    return;
  }

  ESP_LOGW("Interpreter", "Unknown command");
}
