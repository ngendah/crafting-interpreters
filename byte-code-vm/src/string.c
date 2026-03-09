#include <string.h>

#include "string.h"

int min(int a, int b) { return a >= b ? b : a; }

const String _(const char *str) {
  return (String){
      .str = str,
      .length = strlen(str),
  };
}

const int cmp(const String a, const String b) {
  return memcmp(a.str, b.str, min(a.length, b.length));
}
