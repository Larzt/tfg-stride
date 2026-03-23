#include <string>

int parseNumber(const std::string &val)
{
  if (val.find("0x") == 0)
    return std::stoi(val, nullptr, 16);
  return std::stoi(val);
}
