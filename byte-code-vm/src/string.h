#pragma once

#include <stddef.h>

typedef struct {
  const char *const str;
  size_t length;
} String;

const String _(const char *str);
const int cmp(const String a, const String b);
