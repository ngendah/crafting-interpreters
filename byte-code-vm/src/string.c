#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "string.h"

const hash_t string_hash(const char *str, size_t length) {
  // FNV-1a: pg? 589
  hash_t hash = 2'166'136'261u;
  char *ptr = (char *)str;
  const char *pend = &str[length];
  while (ptr != pend) {
    hash ^= (byte_t)*ptr;
    hash *= 16'777'619;
    ptr++;
  }
  return hash;
}

int min(int a, int b) { return a >= b ? b : a; }

const string_t _(const char str[]) {
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

const strtd_r strtd(const char *const str, size_t length) {
  if (length == 0)
    return (strtd_r){.value = 0, .err = true};
  double value = 0, exponent = 1;
  const char *const beg = str;
  char *end = (char *)&str[length - 1], cval[] = {'\0', '\0'};
  char *it = (char *)end;
  bool isNeg = false, err = false, decimal = false;
  auto len = length - 1;
  while (it >= beg) {
    switch (*it) {
    case '+':
      break;
    case '-':
      isNeg = true;
      break;
    case 'E':
    case 'e':
      exponent = exp10(isNeg ? -value : value);
      end = &it[-1];
      value = 0;
      isNeg = false;
      len = end - beg;
      break;
    case '.':
      if (!decimal) {
        decimal = true;
        value = value / exp10(end - it);
        len = (&it[-1]) - beg;
      } else {
        err = true;
      }
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      cval[0] = *it;
      value += atoi(&cval[0]) * exp10(len - (it - beg));
      break;
    default:
      err = true;
      break;
    }
    it--;
  }
  return (strtd_r){.value = exponent * (isNeg ? -value : value), .err = err};
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
