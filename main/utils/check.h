#include <iostream>
#include <string>
#include "colors.h"

int passed = 0;
int failed = 0;

template <typename T>
void check(const T &actual, const T &expected, const std::string &msg)
{
  if (actual != expected)
  {
    std::cerr << RED << "FAILED: " << msg
              << " | expected: " << expected
              << " got: " << actual << std::endl;
    failed++;
  }
  else
  {
    std::cout << GREEN << "PASSED: " << msg << std::endl;
    passed++;
  }
}
