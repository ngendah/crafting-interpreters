#include "fmt/core.h"
#include "parser.hpp"
#include "scanner.hpp"

int main() {
  auto scanner = Scanner("d = a * b / c");
  auto tokens = scanner.scan();
  auto result = Parser(tokens).parse();
  fmt::print("Expression={}\n", result->to_string());
  return 0;
}
