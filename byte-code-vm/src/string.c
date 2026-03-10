#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "string.h"

const hash_t string_hash(const char *str, size_t length) {
  // FNV-1a: pg? 589
  hash_t hash = 2166136261u;
  char *ptr = (char *)str;
  const char *pend = &str[length];
  while (ptr != pend) {
    hash ^= (byte_t)*ptr;
    hash *= 16777619;
    ptr++;
  }
  return hash;
}

int min(int a, int b) { return a >= b ? b : a; }

const string_t _(const char *str) {
  const auto len = strlen(str);
  const auto hash = string_hash(str, len);
  return (string_t){
      .str = str, .length = len, .object = {.is_marked = false, .hash = hash}};
}

const string_t _T(const char *str, size_t length) {
  const auto hash = string_hash(str, length);
  return (string_t){.str = str,
                    .length = length,
                    .object = {.is_marked = false, .hash = hash}};
}

typedef struct {
  double value;
  bool err;
} strtd_r;

const strtd_r strtd(const char *str, size_t length) {
  if (str == &str[length])
    return (strtd_r){.value = 0, .err = true};
  double value = 0;
  const char *beg = (char *)str, *end = (char *)&str[length - 1];
  char *it = (char *)&str[length - 1], cval[] = {'\0', '\0'};
  bool isNeg = false, err = false, decimal = false;
  auto len = length - 1;
  while (it >= beg) {
    if (*it == '+' || (isNeg = *it == '-')) {
      it--;
      continue;
    }
    if (*it == '.' && !decimal) {
      decimal = true;
      len = it - beg > 0 ? (it - beg) - 1 : 0;
      value = value / exp10(end - it);
      it--;
      continue;
    }
    if (isdigit(cval[0] = *it) != 0) {
      value += atoi(&cval[0]) * exp10(len - (it - beg));
    } else {
      err = true;
    }
    it--;
  }
  return (strtd_r){.value = isNeg ? -value : value, .err = err};
}

const double string_as_double(const string_t str) {
  auto value = strtd(str.str, str.length);
  return value.value;
}

const int string_compare(const string_t a, const string_t b) {
  return memcmp(a.str, b.str, min(a.length, b.length));
}

const string_t string_concatenate(const string_t a, const string_t b) {
  const size_t len = a.length + b.length;
  char *ptr = (char *)new(len * sizeof(char));
  memset(ptr, '\0', len);
  strncpy(ptr, a.str, a.length);
  strncat(ptr, b.str, b.length);
  return _T(ptr, len);
}
