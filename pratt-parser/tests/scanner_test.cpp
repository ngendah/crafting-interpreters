#include "scanner.hpp"
#include <cassert>

int main() {
  auto scanner = Scanner("1+1+2");
  auto tokens = scanner.scan();
  assert(tokens.size() == 5);
  return 0;
}
